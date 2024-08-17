#include "src/ui/play.hpp"

#include "src/core/controller.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/is-viewer.hpp"
#include "src/core/libpl.hpp"
#include "src/core/logging.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/sm64.hpp"
#include "src/core/special-groups.hpp"
#include "src/core/time.hpp"
#include "src/db/data-provider.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/rhdc/ui/rhdc-save-sync-dialog.hpp"
#include "src/rhdc/web/api.hpp"

#include "src/ui/is-viewer-window.hpp"
#include "src/ui/multiplayer-controller-select-dialog.hpp"
#include "src/ui/now-playing-window.hpp"
#include "src/ui/process-awaiter.hpp"
#include <QCheckBox>
#include <QCoreApplication>
#include <QFile>
#include <QMessageBox>
#include <ios>
#include <memory>
#include <unordered_map>

#ifdef _WIN32
#include <QTimer>
#include <memory>
#endif

static const char *s_crashMessageParallel = QT_TRANSLATE_NOOP(
    "Game", ""
            "The emulator exited shortly after launching. If you are able to "
            "launch other ROMs without issues, then "
            "this ROM may contain invalid or unsafe RSP microcode that cannot "
            "be run on console accurate graphics "
            "plugins. Alternatively, if you have a very old onboard graphics "
            "card, it is possible that Vulkan is not "
            "supported on your system. In either case, using another graphics "
            "plugin might resolve the issue.");

static const char *s_crashMessageAngrylion = QT_TRANSLATE_NOOP(
    "Game", ""
            "The emulator exited shortly after launching. If you are able to "
            "launch other ROMs without issues, then "
            "this ROM may contain invalid or unsafe RSP microcode that cannot "
            "be run on console accurate graphics "
            "plugins. If this is the case, try running the ROM with another "
            "graphics plugin instead.");

static const char *s_crashMessageDefault = QT_TRANSLATE_NOOP(
    "Game", ""
            "The emulator exited shortly after launching. If you are able to "
            "launch other ROMs without issues, then "
            "this ROM is most likely corrupt.");

static ControllerProfile
getControllerProfile(InputDriver driver, const ConnectedGamepad &controller) {
  if (controller.id < 0) {
    return FileController::loadLastControllerProfile(driver);
  }

  const HashMap<Uuid, string> mappings =
      FileController::loadControllerMappings(driver);
  if (mappings.count(controller.info.uuid) > 0) {
    const string &activeProfile = mappings.at(controller.info.uuid);

    const std::map<string, ControllerProfile> profiles =
        FileController::loadControllerProfiles(driver);
    if (profiles.count(activeProfile) > 0) {
      return profiles.at(activeProfile);
    }
  }

  return DefaultProfile::get(driver,
                             getControllerType(controller.info.controllerId));
}

static ushort getStarCountFromSaveFile(const fs::path &saveFilePath,
                                       const string &hackId) {
  if (StarLayout::hasLayout(hackId)) {
    StarLayout layout;
    if (StarLayout::tryLoadLayout(hackId, layout)) {
      return layout.countStars(saveFilePath);
    }
  }

  InputFile saveFile(saveFilePath, true);
  SM64::SaveFile saveData = SM64::SaveFile::read(saveFile);
  if (FileController::loadRhdcSettings().checkAllSaveSlots) {
    ushort starCount = 0;
    for (int i = 0; i < 4; i++) {
      const ushort slotStars = (ushort)saveData.slot(i).countStars();
      starCount = slotStars > starCount ? slotStars : starCount;
    }
    return starCount;
  } else {
    return (ushort)saveData.slot(0).countStars();
  }
}

static inline ushort getStarCount(const RomFile &romFile,
                                  const string &hackId) {
  const fs::path saveFilePath = RetroArch::getSaveFilePath(romFile.path);
  if (!fs::existsSafe(saveFilePath))
    return 0;

  return getStarCountFromSaveFile(saveFilePath, hackId);
}

static inline string getRomName(const RomFile &romFile,
                                const RomInfo &romInfo) {
  RhdcHack hack;
  if (DataProvider::tryFetchRhdcHackByChecksum(romInfo.sha1, &hack)) {
    return hack.info.name;
  } else if (!romInfo.name.empty()) {
    return romInfo.name;
  } else {
    return romFile.path.stem().u8string();
  }
}

static bool playGame(const RomFile &romFile, const RomInfo &romInfo,
                     const std::vector<PlayerController> &players,
                     bool bindSavestate,
                     const std::function<void(void)> &callback,
                     const StarLayout *testLayout, bool waitForRhdcSync,
                     bool inputBindingError) {
  string groupId;
  bool syncSaves = false;

  if (DataProvider::tryFetchHackIdByChecksum(romInfo.sha1, groupId) &&
      DataProvider::groupSaveEnabled(groupId)) {
    string storedGroupId;
    if (DataProvider::tryGetSaveGroup(romInfo.sha1, storedGroupId) &&
        groupId == storedGroupId) {
      syncSaves = true;
    } else if (testLayout == nullptr) {
      const std::vector<fs::path> romPaths =
          DataProvider::fetchRomPathsForRhdcHack(groupId);

      HashMap<string, fs::path> uniqueSaves;
      HashMap<fs::path, fs::path> savePathToRomPathMap;

      for (const fs::path &romPath : romPaths) {
        const fs::path saveFilePath =
            RetroArch::getIndividualSaveFilePath(romPath);
        if (fs::existsSafe(saveFilePath)) {
          const string sha1 = Sha1::compute(saveFilePath);
          uniqueSaves[sha1] = saveFilePath;
          savePathToRomPathMap[saveFilePath] = romPath;
        }
      }

      if (uniqueSaves.size() == 1) {
        const fs::path groupSaveFile = RetroArch::getGroupSaveFilePath(groupId);
        const fs::path &soloSaveFile = uniqueSaves.begin()->second;

        fs::error_code err;
        fs::remove(groupSaveFile, err);
        err.clear();
        fs::rename(soloSaveFile, groupSaveFile, err);

        if (!err) {
          // Remove any other duplicate save files from other versions
          for (const fs::path &romPath : romPaths) {
            fs::remove(RetroArch::getIndividualSaveFilePath(romPath), err);
          }
        }
      } else if (uniqueSaves.size() > 1) {
        std::vector<SaveFileInfo> saveFileInfos;
        saveFileInfos.reserve(uniqueSaves.size());

        foreach_cvalue(saveFile, uniqueSaves) {
          string versionSource = DataProvider::getVersionNameFromRomPath(
              savePathToRomPathMap.at(saveFile));
          const size_t i = versionSource.find_last_of('.');
          if (i != string::npos) {
            versionSource = versionSource.substr(0, i);
          }

          saveFileInfos.push_back(
              {saveFile, versionSource, RomUtil::getLastModified(saveFile),
               getStarCountFromSaveFile(saveFile, groupId)});
        }

        const fs::path masterSaveFile = RhdcSaveSyncDialog::run(saveFileInfos);
        if (masterSaveFile.empty()) {
          callback();
          return false;
        }

        const fs::path groupSaveFile = RetroArch::getGroupSaveFilePath(groupId);

        fs::error_code err;
        fs::remove(groupSaveFile, err);
        err.clear();
        fs::rename(masterSaveFile, groupSaveFile, err);

        if (!err) {
          for (const fs::path &romPath : romPaths) {
            fs::remove(RetroArch::getIndividualSaveFilePath(romPath), err);
          }
        }
      }

      DataProvider::addAllVersionsToSaveGroup(groupId);
      syncSaves = true;
    }
  } else if (testLayout == nullptr &&
             DataProvider::tryGetSaveGroup(romInfo.sha1, groupId)) {
    const fs::path groupSaveFile = RetroArch::getGroupSaveFilePath(groupId);
    const fs::path soloSaveFile =
        RetroArch::getIndividualSaveFilePath(romFile.path);

    fs::error_code err;
    if (fs::existsSafe(groupSaveFile)) {
      fs::remove(soloSaveFile, err);
      err.clear();
      fs::copy_file(groupSaveFile, soloSaveFile, err);
    }

    DataProvider::removeFromSaveGroup(groupId, romInfo.sha1);
    if (!err && DataProvider::isSaveGroupEmpty(groupId)) {
      fs::remove(groupSaveFile, err);
    }
  }

  const AppSettings &settings = FileController::loadAppSettings();
  const EmulatorCore emulatorCore =
      (romInfo.emulator == EmulatorCore::UseDefault) ? settings.defaultEmulator
                                                     : romInfo.emulator;

  std::shared_ptr<LibplHandler> libplHandler = std::make_shared<LibplHandler>();
  std::shared_ptr<ISViewer> isViewer = std::make_shared<ISViewer>();
  std::shared_ptr<IsViewerWindow> isViewerWindow;

  GfxPlugin gfxPlugin;
  AsyncProcess *emulator = new AsyncProcess();
  try {
    if (settings.enableIsViewer && emulatorCore == EmulatorCore::ParallelN64) {
      isViewerWindow = std::make_shared<IsViewerWindow>();
      QObject::connect(isViewer.get(), &ISViewer::messageReceived,
                       isViewerWindow.get(), &IsViewerWindow::logMessage);
      QObject::connect(isViewer.get(), &ISViewer::error, isViewerWindow.get(),
                       &IsViewerWindow::error);
      isViewer->start();
    }

    const int libplStatus =
        libplHandler->initialize(); // FIXME: only do this for ParallelN64
    if (libplStatus == 0)
      libplHandler->connect(romInfo);
    *emulator = RetroArch::launchRom(
        romFile.path, settings, players, romInfo, bindSavestate,
        syncSaves ? groupId : ""s, &gfxPlugin, isViewer->getPipeFilePath(),
        libplStatus ? ""s : libplHandler->inputPipePath(),
        libplStatus ? ""s : libplHandler->outputPipePath(), inputBindingError);
  } catch (...) {
    QMessageBox::critical(
        nullptr, QCoreApplication::translate("Game", "Emulator Missing"),
        QCoreApplication::translate(
            "Game",
            "Failed to launch emulator. It does not appear to be installed."));
    isViewer->stop();
    callback();
    return false;
  }

  if (testLayout || !settings.hideWhenPlaying) {
    NowPlayingWindow::open(romFile.path, romInfo.sha1,
                           romInfo.playTime, testLayout);
  }

  if (settings.enableIsViewer && emulatorCore == EmulatorCore::ParallelN64) {
    isViewerWindow->show();
  }
  ProcessAwaiter::QtSafeAwait(emulator, [=]([[maybe_unused]] int64 exitCode,
                                            int64 runtimeMs) {
    libplHandler->disconnect();
    if (settings.enableIsViewer && emulatorCore == EmulatorCore::ParallelN64) {
      isViewer->stop();
      isViewerWindow->close();
    }

    delete emulator;
    NowPlayingWindow::close();

    if (syncSaves && !groupId.empty()) {
      fs::error_code err;
      fs::path tempRomPath = RetroArch::getTemporaryRomPath() / groupId;
      tempRomPath.replace_extension(romFile.path.extension());
      fs::remove(tempRomPath, err);
    }

    int64 newPlayTime = romInfo.playTime + runtimeMs;
    if (runtimeMs > 0) {
      const int64 lastPlayed = Time::nowMs();
      DataProvider::updatePlayTime(romInfo.sha1, lastPlayed, newPlayTime);

      RhdcHack hack;
      if (testLayout == nullptr &&
          DataProvider::tryFetchRhdcHackByChecksum(romInfo.sha1, &hack)) {
        const string hackId = hack.info.hackId;
        ushort starProgress = hack.progress.stars;

        const int64 maxPlayTime = DataProvider::getPlayTimeByHackId(hackId);
        if (maxPlayTime > newPlayTime) {
          DataProvider::suggestRhdcHackPlayTime(hackId, maxPlayTime);
          newPlayTime = maxPlayTime;
        }

        const bool hackHasStars = hack.info.starCount > 0;
        if (hackHasStars) {
          starProgress = getStarCount(romFile, hack.info.hackId);
          starProgress = starProgress > hack.info.starCount
                             ? hack.info.starCount
                             : starProgress;
          DataProvider::updateRhdcStarProgress(
              hackId, starProgress, starProgress == hack.info.starCount);

          if (newPlayTime >= 300000 || starProgress > 0) {
            DataProvider::removeFromGroup(romInfo.sha1,
                                          SpecialGroups::WantToPlay);
            if (starProgress == hack.info.starCount) {
              DataProvider::removeFromGroup(romInfo.sha1,
                                            SpecialGroups::InProgress);
              DataProvider::addToGroup(romInfo.sha1, SpecialGroups::InProgress);
            } else {
              DataProvider::addToGroup(romInfo.sha1, SpecialGroups::InProgress);
            }
          }
        }

        std::shared_ptr<bool> rhdcPlayTimeUpdated(new bool(false));
        std::shared_ptr<bool> rhdcProgressUpdated(new bool(false));

        const string sha1 = romInfo.sha1;
        RhdcApi::getStarpowerAsync(
            hackId,
            [=](Starpower &starpower) {
              if (starpower.playTime > newPlayTime) {
                DataProvider::suggestRhdcHackPlayTime(hackId,
                                                      starpower.playTime);
                *rhdcPlayTimeUpdated = true;
              } else if (starpower.playTime < newPlayTime) {
                RhdcApi::submitPlayTimeAsync(
                    hackId, newPlayTime,
                    [rhdcPlayTimeUpdated]() { *rhdcPlayTimeUpdated = true; },
                    [rhdcPlayTimeUpdated](ApiErrorType) {
                      *rhdcPlayTimeUpdated = true;
                    });
              } else {
                *rhdcPlayTimeUpdated = true;
              }

              if (hackHasStars) {
                if (starpower.progress.stars > starProgress) {
                  DataProvider::updateRhdcStarProgress(
                      hackId, starpower.progress.stars,
                      starpower.progress.completed);
                  *rhdcProgressUpdated = true;
                } else if (hack.info.submitStarpower &&
                           starpower.progress.stars < starProgress) {
                  RhdcApi::submitStarProgressAsync(
                      hackId, starProgress,
                      [rhdcProgressUpdated]() { *rhdcProgressUpdated = true; },
                      [rhdcProgressUpdated](ApiErrorType err) {
                        *rhdcProgressUpdated = true;
                        RhdcApi::logApiError("Failed to submit hack progress")(
                            err);
                      });
                } else {
                  *rhdcProgressUpdated = true;
                }
              } else {
                *rhdcProgressUpdated = true;
              }
            },
            [=](ApiErrorType apiError) {
              if (apiError != ApiErrorType::NotFound) {
                *rhdcPlayTimeUpdated = true;
                *rhdcProgressUpdated = true;
                RhdcApi::logApiError("Failed to fetch hack progress")(apiError);
                return;
              }

              RhdcApi::submitPlayTimeAsync(
                  hackId, newPlayTime,
                  [rhdcPlayTimeUpdated]() { *rhdcPlayTimeUpdated = true; },
                  [rhdcPlayTimeUpdated](ApiErrorType) {
                    *rhdcPlayTimeUpdated = true;
                  });

              RhdcApi::submitStarProgressAsync(
                  hackId, starProgress,
                  [rhdcProgressUpdated]() { *rhdcProgressUpdated = true; },
                  [rhdcProgressUpdated](ApiErrorType err) {
                    *rhdcProgressUpdated = true;
                    RhdcApi::logApiError("Failed to submit hack progress")(err);
                  });
            });

        if (waitForRhdcSync && !NowPlayingWindow::shouldReload()) {
          while (!*rhdcPlayTimeUpdated || !*rhdcProgressUpdated) {
            QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents |
                                            QEventLoop::ExcludeUserInputEvents);
          }
        }
      }
    }

    if (NowPlayingWindow::shouldReload()) {
      RomFile newRomFile;
      RomInfo newRomInfo;
      if (DataProvider::tryFetchRomByPath(romFile.path, true, &newRomInfo,
                                          &newRomFile)) {
        playGame(newRomFile, newRomInfo, players, bindSavestate, callback,
                 testLayout, waitForRhdcSync, inputBindingError);
        return;
      }
    }

    callback();
  });

  return true;
}

static inline bool usesTwoPorts(const Uuid &inputModeId) {
  const std::map<Uuid, InputMode> inputModes = FileController::loadInputModes();
  if (inputModes.count(inputModeId) > 0) {
    return inputModes.at(inputModeId).usesTwoPorts();
  }

  return false;
}

bool Game::play(const RomFile &romFile, const RomInfo &romInfo,
                bool multiplayer, bool waitForRhdcSync,
                const std::function<void()> &callback,
                const StarLayout *testLayout) {
  const InputDriver inputDriver = FileController::loadAppSettings().inputDriver;
  std::vector<PlayerController> players;
  players.reserve(multiplayer ? 4 : 1);

  bool inputBindingError = false;
  bool bindSavestate;
  if (multiplayer) {
    try {
      MultiplayerControllerSelectDialog dialog(
          usesTwoPorts(romInfo.inputModeId));
      if (dialog.exec() != QDialog::Accepted) {
        callback();
        return false;
      }

      const std::array<ConnectedGamepad, 4> controllers =
          dialog.getControllers();
      for (size_t i = 0; i < 4; i++) {
        if (controllers[i].id < 0)
          continue;

        while (i > players.size())
          players.push_back({DefaultProfile::get(inputDriver), Uuid()});
        players.push_back({getControllerProfile(inputDriver, controllers[i]),
                           controllers[i].info.uuid});
      }

      if (players.empty()) {
        players.push_back(
            {FileController::loadLastControllerProfile(inputDriver), Uuid()});
      }

      bindSavestate = dialog.canBindSavestates();
    } catch (const std::exception &ex) {
      logError("Failed to fetch connected controllers: "s + ex.what());
      inputBindingError = true;
      bindSavestate = true;
      players.push_back(
          {FileController::loadLastControllerProfile(inputDriver), Uuid()});
    }
  } else {
  }

  size_t highestMousePort = 0;
  for (size_t i = 3; i > 0; i--) {
    if (romInfo.inputTypes[i] == N64InputType::Mouse) {
      highestMousePort = i;
      break;
    }
  }

  while (players.size() <= highestMousePort) {
    players.push_back({DefaultProfile::get(inputDriver), Uuid()});
  }

  return playGame(romFile, romInfo, players, bindSavestate, callback,
                  testLayout, waitForRhdcSync, inputBindingError);
}
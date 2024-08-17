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

  const AppSettings &settings = FileController::loadAppSettings();
  const EmulatorCore emulatorCore =
      (romInfo.emulator == EmulatorCore::UseDefault) ? settings.defaultEmulator
                                                     : romInfo.emulator;

  std::shared_ptr<LibplHandler> libplHandler = std::make_shared<LibplHandler>();
  std::shared_ptr<ISViewer> isViewer = std::make_shared<ISViewer>();
  std::shared_ptr<IsViewerWindow> isViewerWindow;

  GfxPlugin gfxPlugin;
  AsyncProcess *emulator = new AsyncProcess();

  if (testLayout || !settings.hideWhenPlaying) {
    NowPlayingWindow::open(romFile.path, romInfo.sha1,
                           romInfo.playTime, testLayout);
  }

  if (settings.enableIsViewer && emulatorCore == EmulatorCore::ParallelN64) {
    isViewerWindow->show();
  }

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
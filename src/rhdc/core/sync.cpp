#include "src/rhdc/core/sync.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/logging.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/rom.hpp"
#include "src/core/sm64.hpp"
#include "src/core/special-groups.hpp"
#include "src/core/uuid.hpp"
#include "src/db/data-provider.hpp"
#include "src/db/transaction.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/ui/toast.hpp"
#include <QCoreApplication>
#include <QFile>
#include <algorithm>

static inline bool shouldUseWidescreen(const RhdcHackVersionExt &version) {
  return (Flags::has(version.pluginFlags, PluginFlags::Widescreen) &&
          !FileController::loadRhdcSettings().ignoreWidescreenHint);
}

static void addHackVersion(const FollowedHack &hack,
                           const RhdcHackVersionExt &version) {
  EmulatorCore emulator = EmulatorCore::UseDefault;
  GfxPlugin parallelPlugin = GfxPlugin::UseDefault;
  GfxPlugin mupenPlugin = GfxPlugin::UseDefault;

  if (version.plugin.has_value()) {
    const GfxPlugin recommendedPlugin = version.plugin.value();
    switch (recommendedPlugin) {
    case GfxPlugin::Angrylion:
    case GfxPlugin::ParaLLEl:
    case GfxPlugin::GLideN64:
      parallelPlugin = mupenPlugin = recommendedPlugin;
      break;
    default:
      emulator = EmulatorCore::ParallelN64;
      parallelPlugin = recommendedPlugin;
      mupenPlugin = GfxPlugin::GLideN64;
      break;
    }
  }

  DataProvider::addRomInfo(
      {version.sha1,
       "",
       "",
       emulator,
       parallelPlugin,
       mupenPlugin,
       Flags::has(version.pluginFlags, PluginFlags::UpscaleTexrects),
       true,
       Flags::has(version.pluginFlags, PluginFlags::EmulateFramebuffer),
       Flags::has(version.pluginFlags, PluginFlags::AcccurateDepthCompare),
       Flags::has(version.pluginFlags, PluginFlags::EmulateRsp),
       0,
       0,
       std::set<string>(hack.groups.begin(), hack.groups.end()),
       !Flags::has(version.hackFlags, RhdcHackFlag::NoOverclock),
       false,
       0,
       Flags::has(version.hackFlags, RhdcHackFlag::DualAnalog)
           ? DefaultInputModes::DualAnalog.id
           : DefaultInputModes::Normal.id,
       shouldUseWidescreen(version) ? WidescreenMode::ViewportHack
                                    : WidescreenMode::Off,
       Flags::has(version.hackFlags, RhdcHackFlag::BigEEPROM),
       false,
       UNCHECKED_INPUT_TYPES,
       Flags::has(version.hackFlags, RhdcHackFlag::SupportsSD) ? "?" : ""});
}

static inline const RhdcHackVersionExt &
getLatestVersion(const std::vector<RhdcHackVersionExt> &versions) {
  for (const RhdcHackVersionExt &version : versions) {
    if (!version.archived)
      return version;
  }
  return versions.at(0);
}

void RHDC::sync(const std::function<void(bool)> &callback) {
  using namespace RHDC;

  RhdcApi::getFollowedHacksAsync(
      [callback](HashMap<string, FollowedHack> &hacks) {
        SqlTransaction transaction;

        DataProvider::clearRhdcHacks();
        const HashSet<string> knownHashes =
            DataProvider::fetchAllKnownChecksums();
        std::vector<string> undownloadedHacks;

        fs::error_code err;
        fs::create_directories(BaseDir::data() / _NFS("layouts"), err);
        fs::create_directories(BaseDir::data() / _NFS("jsml"), err);

        foreach_cvalue(hack, hacks) {
          DataProvider::addOrUpdateRhdcHack(hack.info);
          bool knownHack = false;

          int i = 0;
          for (const RhdcHackVersionExt &version : hack.versions) {
            DataProvider::addRhdcHackVersion(
                hack.info.hackId, version.sha1, version.name, i++,
                version.plugin.value_or(GfxPlugin::UseDefault),
                version.hackFlags, version.archived);

            if (knownHashes.count(version.sha1) > 0) {
              knownHack = true;
            } else {
              addHackVersion(hack, version);
            }
          }

          for (const string &author : hack.authors) {
            DataProvider::addRhdcHackAuthor(hack.info.hackId, author);
          }

          if (!knownHack) {
            undownloadedHacks.push_back(hack.info.hackId);
          } else {
            std::set<string> localGroups =
                DataProvider::fetchGroupsForRhdcHack(hack.info.hackId);
            for (const string &group : hack.groups) {
              if (localGroups.count(group) > 0) {
                localGroups.erase(group);
              } else {
                DataProvider::addRhdcHackToGroup(hack.info.hackId, group);
              }
            }

            for (const string &group : localGroups) {
              if (group == SpecialGroups::Uncategorized)
                continue;
              DataProvider::removeRhdcHackFromGroup(hack.info.hackId, group);
            }
          }
        }
        transaction.commit();

        for (const string &hackId : undownloadedHacks) {
          const FollowedHack &hack = hacks.at(hackId);
          if (hack.versions.empty())
            continue;

          const RhdcHackVersionExt &latestVersion =
              getLatestVersion(hack.versions);
          if (latestVersion.archived)
            continue;

          DataProvider::addRomFile(
              {RhdcApi::getDownloadUrl(hack.info.slug, latestVersion.name), 0,
               latestVersion.sha1, false});
        }

        /* Starpower */ {
          const HashMap<string, StarpowerExt> clientStarpower =
              DataProvider::fetchAllStarpower();
          SqlTransaction transaction;

          for (const auto &[hackId, hack] : hacks) {
            const Starpower &starpower = hack.starpower;

            if (clientStarpower.count(hackId) <= 0) {
              DataProvider::updateRhdcProgress(hackId, starpower.progress);
              DataProvider::suggestRhdcHackPlayTime(hackId, starpower.playTime);
              continue;
            }

            const StarpowerExt &localStarpower = clientStarpower.at(hackId);
            const RhdcHackProgress &clientProgress = localStarpower.progress;
            const RhdcHackProgress &serverProgress = starpower.progress;

            const RhdcHackProgress newProgress = {
                serverProgress.rating, serverProgress.difficulty,
                std::max(clientProgress.stars, serverProgress.stars),
                clientProgress.completed || serverProgress.completed};

            if (clientProgress != newProgress) {
              DataProvider::updateRhdcProgress(hackId, newProgress);
            }

            if (hack.info.submitStarpower && serverProgress != newProgress) {
              if (localStarpower.starCount > 0) {
                RhdcApi::submitStarProgressAsync(
                    hackId, newProgress.stars, []() {},
                    RhdcApi::logApiError("Failed to submit hack progress"));
              } else {
                RhdcApi::submitCompletionAsync(
                    hackId, newProgress.completed, []() {},
                    RhdcApi::logApiError("Failed to submit hack progress"));
              }
            }

            if (starpower.playTime < localStarpower.playTime) {
              RhdcApi::submitPlayTimeAsync(
                  hackId, localStarpower.playTime, []() {},
                  [](ApiErrorType) {});
            } else if (starpower.playTime > localStarpower.playTime) {
              DataProvider::suggestRhdcHackPlayTime(hackId, starpower.playTime);
            }
          }
          transaction.commit();
        }

        callback(true);
      },
      [callback](ApiErrorType err) {
        if (err != ApiErrorType::NotAuthorized) {
          ToastMessageManager::display(
              ToastType::Error,
              QCoreApplication::translate(
                  "RhdcSync", "Failed to connect to romhacking.com"));
        }
        callback(false);
      });
}

void RHDC::moveRhdcFolder(const fs::path &oldDir, const fs::path &newDir) {
  fs::error_code err;
  fs::create_directories(newDir, err);
  if (!fs::existsSafe(oldDir))
    return;
  for (const auto &p : fs::directory_iterator(oldDir)) {
    if (!fs::isRegularFileSafe(p.path()))
      continue;
    if (p.path().extension() != ".z64")
      continue;
    fs::rename(p.path(), newDir / p.path().filename(), err);
  }
}

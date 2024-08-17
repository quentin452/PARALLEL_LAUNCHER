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

static bool playGame(const RomFile &romFile, const RomInfo &romInfo,
                     const std::vector<PlayerController> &players,
                     bool bindSavestate,
                     const std::function<void(void)> &callback,
                     const StarLayout *testLayout, bool waitForRhdcSync,
                     bool inputBindingError) {
  const AppSettings &settings = FileController::loadAppSettings();
  const EmulatorCore emulatorCore =
      (romInfo.emulator == EmulatorCore::UseDefault) ? settings.defaultEmulator
                                                     : romInfo.emulator;

  if (testLayout || !settings.hideWhenPlaying) {
    NowPlayingWindow::open(romFile.path, romInfo.sha1,
                           romInfo.playTime, testLayout);
  }

  return true;
}

bool Game::play(const RomFile &romFile, const RomInfo &romInfo,
                bool multiplayer, bool waitForRhdcSync,
                const std::function<void()> &callback,
                const StarLayout *testLayout) {
  const InputDriver inputDriver = FileController::loadAppSettings().inputDriver;
  std::vector<PlayerController> players;
  bool inputBindingError = false;
  bool bindSavestate;
  return playGame(romFile, romInfo, players, bindSavestate, callback,
                  testLayout, waitForRhdcSync, inputBindingError);
}
#ifndef SRC_CORE_FILE_CONTROLLER_HPP_
#define SRC_CORE_FILE_CONTROLLER_HPP_

#include "src/core/controller.hpp"
#include "src/core/rom.hpp"
#include "src/core/settings.hpp"
#include "src/core/updates.hpp"
#include "src/input/input-driver.hpp"
#include "src/input/keyboard.hpp"
#include "src/rhdc/core/rhdc-settings.hpp"
#include "src/ui/ui-state.hpp"
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace FileController {

extern const AppSettings &loadAppSettings();
extern void saveAppSettings(const AppSettings &settings);

extern const std::vector<RomSource> &loadRomSources();
extern void saveRomSources(const std::vector<RomSource> &sources);

extern const std::map<string, ControllerProfile> &
loadControllerProfiles(InputDriver driver);
extern void
saveControllerProfiles(InputDriver driver,
                       const std::map<string, ControllerProfile> &profiles);

extern const HashMap<Uuid, string> &loadControllerMappings(InputDriver driver);
extern void saveControllerMappings(InputDriver driver,
                                   const HashMap<Uuid, string> &mappings);

extern const UiState &loadUiState();
extern void saveUiState(const UiState &state);

extern const std::vector<KeyId> &loadHotkeys();
extern void saveHotkeys(const std::vector<KeyId> &hotkeys);

extern const std::map<Uuid, InputMode> &loadInputModes();
extern void saveInputModes(const std::map<Uuid, InputMode> &modes);

extern const InstalledVersionsInfo &loadInstalledVersions();
extern void saveInstalledVersions(const InstalledVersionsInfo &versions);

extern const RhdcSettings &loadRhdcSettings();
extern void saveRhdcSettings(const RhdcSettings &settings);

extern const ControllerProfile &loadLastControllerProfile(InputDriver driver);
extern void saveLastControllerProfile(InputDriver driver, const string &name);

} // namespace FileController

#endif /* SRC_CORE_FILE_CONTROLLER_HPP_ */

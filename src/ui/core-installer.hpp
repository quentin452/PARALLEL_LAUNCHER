#ifndef SRC_UI_CORE_INSTALLER_HPP_
#define SRC_UI_CORE_INSTALLER_HPP_

#include "src/core/settings.hpp"
#include "src/core/updates.hpp"

namespace CoreInstaller {

extern bool requireCore(EmulatorCore core);
extern void updateParallelSync();
extern void checkForUpdatesAsync();
extern void checkForUpdatesSync(InstalledVersionsInfo &versions);

} // namespace CoreInstaller

#endif /* SRC_UI_CORE_INSTALLER_HPP_ */

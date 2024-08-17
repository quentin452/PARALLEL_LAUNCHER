#ifndef SRC_CORE_RETROARCH_HPP_
#define SRC_CORE_RETROARCH_HPP_

#include "src/core/controller.hpp"
#include "src/core/rom.hpp"
#include "src/polyfill/process.hpp"
#include "src/types.hpp"

namespace RetroArch {

extern ubyte resolveUpscaling(ParallelUpscaling requestedUpscaling,
                              ubyte windowScale) noexcept;

extern AsyncProcess
launchRom(fs::path romPath, const AppSettings &settings,
          const std::vector<PlayerController> &players, const RomInfo &romInfo,
          bool bindSavestate, const string &saveGroup, GfxPlugin *plugin,
          const string &isViewerFile, const string &libplInputPipe,
          const string &libplOutputPipe, bool inputBindingError);

extern fs::path getSaveFilePath(const fs::path &romPath);
extern fs::path getIndividualSaveFilePath(const fs::path &romPath);
extern fs::path getGroupSaveFilePath(const string &groupId);

extern bool isRetroArchInstalled();
extern bool isEmulatorCoreInstalled(EmulatorCore core);

extern fs::path getBasePath();
extern fs::path getCorePath();
extern fs::path getConfigPath();
extern fs::path getExePath();

extern fs::path getTemporaryRomPath();

extern bool resetConfig();
extern void migrateSaves();

} // namespace RetroArch

#endif /* SRC_CORE_RETROARCH_HPP_ */

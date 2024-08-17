#ifndef SRC_CORE_BPS_HPP_
#define SRC_CORE_BPS_HPP_

#include "src/core/filesystem.hpp"
#include "src/core/rom.hpp"

namespace Bps {

enum class BpsApplyError : ubyte {
  None,
  InvalidBps,
  PatchFailed,
  NoBaseRom,
  WriteError
};

extern BpsApplyError tryApplyBps(const fs::path &bpsPath,
                                 fs::path &patchedRomPath);
extern BpsApplyError tryApplyBps(const fs::path &bpsPath,
                                 const fs::path &baseRomPath,
                                 fs::path &patchedRomPath);

} // namespace Bps

#endif /* SRC_CORE_ASYNC_HPP_ */

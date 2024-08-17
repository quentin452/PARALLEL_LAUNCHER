#ifndef SRC_CORE_SAVE_CONVERTER_HPP_
#define SRC_CORE_SAVE_CONVERTER_HPP_

#include "src/core/filesystem.hpp"
#include "src/types.hpp"

namespace SaveConverter {

extern bool importFromProject64(const fs::path &destinationPath,
                                const fs::path &sourcePath);

#ifdef _WIN32
extern void autoImportFromProject64(const fs::path &destinationPath,
                                    const string &internalRomName);
#endif

} // namespace SaveConverter

#endif /* SRC_CORE_SAVE_CONVERTER_HPP_ */

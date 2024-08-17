#ifndef SRC_CORE_ZIP_HPP_
#define SRC_CORE_ZIP_HPP_

#include "src/core/filesystem.hpp"

namespace Zip {

extern bool unzip(const fs::path &zipFilePath, const fs::path &outputDir);

}

#endif /* SRC_CORE_ZIP_HPP_ */

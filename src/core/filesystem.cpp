#include "src/core/filesystem.hpp"

#ifdef __APPLE__

bool fs::tryCopyRecursive(const fs::path &srcDir,
                          const fs::path &destDir) noexcept {
  fs::error_code err;
  if (fs::isDirectorySafe(srcDir)) {
    fs::copy_directory(srcDir, destDir, err);
    for (const fs::directory_entry &node :
         fs::directory_iterator(srcDir, err)) {
      if (!fs::tryCopyRecursive(node.path(),
                                destDir / node.path().filename())) {
        return false;
      }
    }
    return true;
  } else {
    fs::copy(srcDir, destDir, err);
    return !err;
  }
}

#endif

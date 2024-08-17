#ifndef SRC_POLYFILL_FILE_SEARCH_HPP_
#define SRC_POLYFILL_FILE_SEARCH_HPP_

#include "src/core/async.hpp"
#include "src/core/filesystem.hpp"
#include "src/types.hpp"
#include <vector>

namespace FileSearch {

extern std::vector<fs::path> find(const fs::path &directory,
                                  const string &pattern, ubyte maxDepth,
                                  bool followSymlinks, bool ignoreHidden,
                                  CancellationToken &cancellationToken);

}

#endif /* SRC_POLYFILL_FILE_SEARCH_HPP_ */

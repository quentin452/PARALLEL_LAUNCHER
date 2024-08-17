#ifndef SRC_CORE_ROM_FINDER_HPP_
#define SRC_CORE_ROM_FINDER_HPP_

#include "src/core/async.hpp"
#include <functional>

namespace RomFinder {

enum SearchResults { Updated, NoChanges, Cancelled };

extern CancellationToken
scanAsync(const std::function<void(SearchResults)> &qtCallback);

} // namespace RomFinder

#endif /* SRC_CORE_ROM_FINDER_HPP_ */

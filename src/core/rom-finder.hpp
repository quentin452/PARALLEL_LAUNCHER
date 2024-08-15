#ifndef SRC_CORE_ROM_FINDER_HPP_
#define SRC_CORE_ROM_FINDER_HPP_

#include <functional>
#include "src/core/async.hpp"

namespace RomFinder {

	enum SearchResults {
		Updated,
		NoChanges,
		Cancelled
	};

	extern CancellationToken scanAsync(
		const std::function<void(SearchResults)> &qtCallback
	);

}

#endif /* SRC_CORE_ROM_FINDER_HPP_ */

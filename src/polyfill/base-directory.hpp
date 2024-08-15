#ifndef SRC_POLYFILL_BASE_DIRECTORY_HPP_
#define SRC_POLYFILL_BASE_DIRECTORY_HPP_

#include "src/core/filesystem.hpp"
#include "src/types.hpp"

namespace BaseDir {

	extern const fs::path &home();
	extern const fs::path &config();
	extern const fs::path &data();
	extern const fs::path &cache();
	extern const fs::path &temp();
	extern const fs::path &program();
#ifndef __APPLE__
	extern const fs::path &share();
#endif

	extern fs::path expandHome( const fs::path &path );

}

#endif /* SRC_POLYFILL_BASE_DIRECTORY_HPP_ */

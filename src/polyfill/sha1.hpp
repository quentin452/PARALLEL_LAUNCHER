#ifndef SRC_POLYFILL_SHA1_HPP_
#define SRC_POLYFILL_SHA1_HPP_

#include "src/core/filesystem.hpp"

#ifdef _WIN32
#include <stdexcept>

class WinCryptException final : std::runtime_error {

	public:
	WinCryptException() : std::runtime_error(
		"Failed to get the checksum of a rom because of an unexpected error in the Windows cryptography API"
	) {}
	virtual ~WinCryptException() {};

};
#endif

namespace Sha1 {
	extern std::string compute( const fs::path &filePath );
}



#endif /* SRC_POLYFILL_SHA1_HPP_ */

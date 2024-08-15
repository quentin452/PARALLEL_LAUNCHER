#ifndef SRC_POLYFILL_MACOS_APPLE_UTIL_HPP_
#define SRC_POLYFILL_MACOS_APPLE_UTIL_HPP_

namespace AppleUtil {

	extern bool isSilicon() noexcept;
	extern bool shouldUseArmCore();
	extern bool isArmLibrary( const char *path ) noexcept;

}

#endif /* SRC_POLYFILL_MACOS_APPLE_UTIL_HPP_ */

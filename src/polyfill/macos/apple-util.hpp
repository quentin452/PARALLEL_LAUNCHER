#ifndef SRC_POLYFILL_MACOS_APPLE_UTIL_HPP_
#define SRC_POLYFILL_MACOS_APPLE_UTIL_HPP_

namespace AppleUtil {

extern bool isSilicon() noexcept;
extern bool shouldUseArmCore();
extern bool isArmLibrary(const char *path) noexcept;

} // namespace AppleUtil

#endif /* SRC_POLYFILL_MACOS_APPLE_UTIL_HPP_ */

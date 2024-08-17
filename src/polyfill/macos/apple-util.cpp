#if defined(__APPLE__)
#include "src/polyfill/macos/apple-util.hpp"

#include <exception>
#include <cstring>
#include "src/polyfill/file.hpp"
#include "src/types.hpp"

#ifdef __arm64__
bool AppleUtil::isSilicon() noexcept {
	return true;
}
#else

#include <sys/sysctl.h>
#include "src/core/file-controller.hpp"

static inline bool getIsSilicon() noexcept {
	int result = 0;
	size_t rs = sizeof( int );
	return sysctlbyname( "sysctl.proc_translated", &result, &rs, nullptr, 0 ) == 0 && result > 0;
}

bool AppleUtil::isSilicon() noexcept {
	static const bool s_isSilicon = getIsSilicon();
	return s_isSilicon;
}
#endif

bool AppleUtil::shouldUseArmCore() {
	if( FileController::loadAppSettings().forceX86 ) return false;
	return AppleUtil::isSilicon();
}

static const ubyte s_armHeader[4] = {
	0xCA, 0xFE,
	0xBA, 0xBE
};

bool AppleUtil::isArmLibrary( const char *path ) noexcept {
	// I don't know if this check is true in general, but it is for the ParallelN64 core, which is all I care about
	try {
		InputFile library( path, true );
		if( !library.good() ) return false;

		ubyte magicNumber[4];
		library.read( (char*)magicNumber, 4 );
		if( !library.good() ) return false;

		return std::memcmp( magicNumber, s_armHeader, 4 ) == 0;
	} catch( const std::exception& ) {
		return false;
	}
}
#endif //defined(__APPLE__)
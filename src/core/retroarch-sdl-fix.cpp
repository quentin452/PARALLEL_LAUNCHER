#include "src/core/retroarch-sdl-fix.hpp"

#ifdef _WIN32

#include <cstring>
#include "windows.h"
#include "src/core/filesystem.hpp"
#include "src/core/logging.hpp"
#include "src/core/buffer.hpp"
#include "src/polyfill/base-directory.hpp"

bool RetroArch::hasCorrectSdlLibrary() {
	static const fs::path plSdlPath = BaseDir::program() / L"SDL2.dll";
	static const fs::path retroSdlPath = BaseDir::data() / L"retroarch" / L"SDL2.dll";

	if( !fs::existsSafe( plSdlPath ) || !fs::existsSafe( retroSdlPath ) ) {
		logError( "Failed to get SDL library path. Controller bindings may not work correctly on all controller types." );
		return true;
	}

	fs::error_code err;
	if( fs::file_size( plSdlPath, err ) != fs::file_size( retroSdlPath, err ) ) return false;

	const size_t psis = (size_t)GetFileVersionInfoSizeW( plSdlPath.wstring().c_str(), nullptr );
	const size_t rsis = (size_t)GetFileVersionInfoSizeW( retroSdlPath.wstring().c_str(), nullptr );
	if( psis != rsis ) return false;

	Buffer piBuffer( psis );
	Buffer riBuffer( rsis );

	if( !GetFileVersionInfoW( plSdlPath.wstring().c_str(), 0, (uint)psis, (void*)piBuffer.data() ) ) return false;
	if( !GetFileVersionInfoW( retroSdlPath.wstring().c_str(), 0, (uint)rsis, (void*)riBuffer.data() ) ) return false;

	return std::memcmp( (void*)piBuffer.data(), (void*)riBuffer.data(), psis ) == 0;
}

bool RetroArch::fixSdlLibrary() {
	const fs::path plSdlPath = BaseDir::program() / L"SDL2.dll";
	const fs::path retroSdlPath = BaseDir::data() / L"retroarch" / L"SDL2.dll";

	if( !fs::existsSafe( plSdlPath ) || !fs::existsSafe( retroSdlPath.parent_path() ) ) {
		return false;
	}

	fs::error_code err;
	if( fs::existsSafe( retroSdlPath ) ) {
		if( !fs::remove( retroSdlPath, err ) || err ) return false;
	}

	if( !fs::copy_file( plSdlPath, retroSdlPath, err ) || err ) {
		return false;
	}

	return hasCorrectSdlLibrary();
}
#endif

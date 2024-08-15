#ifndef __APPLE__

#include "src/input/sdl-lsjs.hpp"

#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/process.hpp"
#ifdef _WIN32
#include "src/core/file-controller.hpp"
#include "src/polyfill/env.hpp"
#endif

#ifndef _WIN32
static inline string escapePath( const fs::path &path ) {
	string escapedPath = "";
	for( const char c : path.u8string() ) {
		if( c == '\"' || c == '\\' || c == '`' || c == '$' ) {
			escapedPath += '\\';
		}
		escapedPath += c;
	}
	return escapedPath;
}
#endif

std::vector<Uuid> lsjs::tryGetPortOrder( bool &success ) {
	success = false;
	try {
	#ifdef _WIN32
		Environment::set( "SDL_JOYSTICK_RAWINPUT", FileController::loadAppSettings().sdlRawInput ? "1" : "0" );
		const string lsjsCmd = (BaseDir::share() / L"parallel-launcher-lsjs.exe").u8string();
	#else
		const string lsjsCmd = escapePath( (BaseDir::share() / "parallel-launcher-lsjs").u8string() );
	#endif

		string uuidList;
		if( !Process::tryGetOutput( lsjsCmd, uuidList ) ) {
			return std::vector<Uuid>();
		}

		std::vector<Uuid> uuids;
		size_t lineStart = 0;
		while( lineStart < uuidList.length() ) {
			size_t lineEnd = uuidList.find( '\n', lineStart );
			if( lineEnd == string::npos ) {
				lineEnd = uuidList.length();
			}

			Uuid uuid;
			if( lineEnd - lineStart >= 36 && Uuid::tryParse( uuidList.substr( lineStart, 36 ), uuid ) ) {
				uuids.push_back( uuid );
			}

			lineStart = lineEnd + 1;
		}

		success = true;
		return uuids;
	} catch( ... ) {
		return std::vector<Uuid>();
	}
}

#endif

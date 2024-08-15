#include "src/polyfill/base-directory.hpp"

#ifdef _WIN32
#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <libloaderapi.h>
#include <exception>
#include <string>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <cstdlib>
#include "src/polyfill/crash.hpp"
#endif

struct Locations {
	fs::path home;
	fs::path config;
	fs::path data;
	fs::path cache;
	fs::path temp;
	fs::path program;
#ifdef __linux__
	fs::path share;
#endif
};

static constexpr char APP_NAME[] = "parallel-launcher";

#ifdef _WIN32

static fs::path fixWindowsPath( const wchar_t *path ) {
	wchar_t fixedPath[MAX_PATH + 1];
	GetLongPathNameW( path, fixedPath, MAX_PATH + 1 );
	fixedPath[MAX_PATH] = '\0';
	return fs::path( fixedPath );
}

static Locations getLocations() {
	wchar_t *buffer;

	if( SHGetKnownFolderPath(
		FOLDERID_Profile,
		KF_FLAG_NO_ALIAS | KF_FLAG_DEFAULT_PATH | KF_FLAG_NOT_PARENT_RELATIVE,
		nullptr,
		&buffer
	) != S_OK ) {
		if( buffer != nullptr ) {
			CoTaskMemFree( buffer );
		}
		throw std::runtime_error( "Failed to get home directory location from Windows." );
	}

	const fs::path home = fixWindowsPath( buffer );
	fs::path appData;

	buffer = nullptr;
	if( SHGetKnownFolderPath(
		FOLDERID_LocalAppData,
		KF_FLAG_CREATE | KF_FLAG_NO_ALIAS | KF_FLAG_DONT_VERIFY | KF_FLAG_DEFAULT_PATH | KF_FLAG_NOT_PARENT_RELATIVE,
		nullptr,
		&buffer
	) == S_OK ) {
		appData = fixWindowsPath( buffer );
	} else {
		appData = home / L"AppData" / L"Local";
	}

	appData = appData / APP_NAME;
	if( buffer != nullptr ) {
		CoTaskMemFree( buffer );
	}

	DWORD result = 0;

	wchar_t currentExePath[261];
	result = GetModuleFileNameW( nullptr, currentExePath, 261 );
	if( result <= 0 || result >= 261 ) {
		throw std::runtime_error( "Failed to get current executable path from Windows." );
	}

	wchar_t tempDirPath[261];
	result = GetTempPathW( 261, tempDirPath );
	if( result <= 0 || result >= 261 ) {
		throw std::runtime_error( "Failed to get temporary directory path from Windows." );
	}

	return Locations{
		home,
		appData / L"config",
		appData / L"data",
		appData / L"cache",
		fixWindowsPath( tempDirPath ),
		fixWindowsPath( currentExePath ).parent_path()
	};
}
#elif __APPLE__
static Locations getLocations() {
	const fs::path home = fs::to_path( std::getenv( "HOME" ) );
	const fs::path appData = home / "Library" / APP_NAME;

	char exePath[4096];
	uint maxPathLen = 4096;
	const int status = _NSGetExecutablePath( exePath, &maxPathLen );
	pl_assert( status == 0 );

	return Locations{
		home,
		appData / "config",
		appData / "data",
		appData / "cache",
		fs::temp_directory_path() / APP_NAME,
		fs::canonical( fs::to_path( exePath ) ).parent_path()
	};
}
#else
static fs::path xdg( const char *varName, const fs::path &homePath, const char *defaultRelativePath ) {
	char *xdgVal = getenv( varName );
	if( xdgVal != nullptr && xdgVal[0] != '\0' ) {
		return fs::path( xdgVal );
	}
	return homePath / defaultRelativePath;
}

static Locations getLocations() {
	const fs::path home = fs::path( std::getenv( "HOME" ) );
	return Locations{
		home,
		xdg( "XDG_CONFIG_HOME", home, ".config" ) / APP_NAME,
		xdg( "XDG_DATA_HOME", home, ".local/share" ) / APP_NAME,
		xdg( "XDG_CACHE_HOME", home, ".cache" ) / APP_NAME,
		fs::temp_directory_path() / APP_NAME,
		fs::read_symlink( "/proc/self/exe" ).parent_path(),
#if defined(FLATPAK_VERSION)
		fs::path( "/app/share/parallel-launcher" )
#elif defined(__linux__)
		fs::path( "/usr/share/parallel-launcher" )
#endif
	};
}
#endif

static inline Locations fetchOrCreateLocations() {
	const Locations locations = getLocations();
	fs::create_directories( locations.config );
	fs::create_directories( locations.data );
	fs::create_directories( locations.cache );
	fs::create_directories( locations.temp );
	return locations;
}

static inline const Locations &lazyGetLocations() {
	static const Locations s_locations = fetchOrCreateLocations();
	return s_locations;
}

const fs::path &BaseDir::home() {
	return lazyGetLocations().home;
}

const fs::path &BaseDir::config() {
	return lazyGetLocations().config;
}

const fs::path &BaseDir::data() {
	return lazyGetLocations().data;
}

const fs::path &BaseDir::cache() {
	return lazyGetLocations().cache;
}

const fs::path &BaseDir::temp() {
	return lazyGetLocations().temp;
}

const fs::path &BaseDir::program() {
	return lazyGetLocations().program;
}

#ifndef __APPLE__
const fs::path &BaseDir::share() {
#if defined(_WIN32) || defined(DEBUG)
	return BaseDir::program();
#else
	return lazyGetLocations().share;
#endif
}
#endif

fs::path BaseDir::expandHome( const fs::path &path ) {
	const std::string pathStr = path.u8string();
	if( pathStr == "~" ) {
		return BaseDir::home();
	}
#ifdef _WIN32
	else if( pathStr.length() >= 2 && pathStr[0] == '~' && (pathStr[1] == '/' || pathStr[1] == '\\' ) ) {
		return BaseDir::home() / fs::path( &path.wstring().c_str()[2] );
	}
#else
	else if( pathStr.length() >= 2 && pathStr[0] == '~' && pathStr[1] == '/' ) {
		return BaseDir::home() / fs::path( &path.c_str()[2] );
	}
#endif
	else return path;
}

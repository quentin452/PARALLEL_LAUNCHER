#ifndef SRC_CORE_FILESYSTEM_HPP_
#define SRC_CORE_FILESYSTEM_HPP_

#ifdef __APPLE__
#include <boost/filesystem.hpp>
#define u8string string
#else
#include <filesystem>
#endif
#include <functional>

#ifdef _WIN32
#include "src/polyfill/windows/unicode.hpp"
#endif

#ifdef __APPLE__
namespace fs = boost::filesystem;

template<> struct std::hash<fs::path> {
	inline size_t operator()(const fs::path &path) const noexcept {
		return std::hash<string>()( path.string() );
	}
};

namespace boost::filesystem {
#else
namespace fs = std::filesystem;

namespace std::filesystem {
#endif

#ifdef _WIN32
	inline path to_path( const std::string &u8string ) {
		return path( Unicode::toUtf16( u8string ) );
	}
#else
	inline fs::path to_path( const std::string &u8string ) {
		return fs::path( u8string );
	}
#endif

#ifdef __APPLE__
	typedef boost::system::error_code error_code;
#else
	typedef std::error_code error_code;
#endif

	inline bool existsSafe( const fs::path &path ) noexcept {
		fs::error_code err;
#if defined(__APPLE__)
		return fs::symlink_status( path, err ).type() != fs::file_type::file_not_found && !err;
#elif defined(_WIN32)
		return fs::status( path, err ).type() != fs::file_type::not_found && !err;
#else
		return fs::symlink_status( path, err ).type() != fs::file_type::not_found && !err;
#endif
	}

	inline bool isRegularFileSafe( const fs::path &path ) noexcept {
		fs::error_code err;
		return fs::is_regular_file( path, err ) && !err;
	}

	inline bool isDirectorySafe( const fs::path &path ) noexcept {
		fs::error_code err;
		return fs::is_directory( path, err ) && !err;
	}

	inline bool isSymlinkSafe( const fs::path &path ) noexcept {
		fs::error_code err;
		return fs::is_symlink( path, err ) && !err;
	}

#ifdef __APPLE__
	extern bool tryCopyRecursive( const fs::path &srcDir, const fs::path &destDir ) noexcept;
#endif

}

#ifdef _WIN32
#define _NFS( str ) L##str
#else
#define _NFS( str ) str
#endif

#endif /* SRC_CORE_FILESYSTEM_HPP_ */

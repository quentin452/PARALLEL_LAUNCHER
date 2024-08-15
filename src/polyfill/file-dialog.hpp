#ifndef SRC_POLYFILL_FILE_DIALOG_HPP_
#define SRC_POLYFILL_FILE_DIALOG_HPP_

#include <initializer_list>
#include <vector>
#include "src/core/filesystem.hpp"

namespace FileDialog {

	struct FileFilter {
		const char *name;
		const char *patterns;
	};

	fs::path getDirectory( const char *dialogTitle );
	fs::path getFile( const char *dialogTitle );
	std::vector<fs::path> getFiles( const char *dialogTitle );
	fs::path getFile( const char *dialogTitle, const std::initializer_list<FileFilter> &filters );
	std::vector<fs::path> getFiles( const char *dialogTitle, const std::initializer_list<FileFilter> &filters );

}



#endif /* SRC_POLYFILL_FILE_DIALOG_HPP_ */

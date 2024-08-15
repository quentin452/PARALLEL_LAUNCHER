#ifndef SRC_POLYFILL_FILE_EXPLORER_HPP_
#define SRC_POLYFILL_FILE_EXPLORER_HPP_

#include "src/core/filesystem.hpp"

namespace FileExplorer {

	extern void showFile( const fs::path &filePath );
	extern void showFolder( const fs::path &folderPath );

}

#endif /* SRC_POLYFILL_FILE_EXPLORER_HPP_ */

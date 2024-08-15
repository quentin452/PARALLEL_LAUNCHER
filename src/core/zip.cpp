#include "src/core/zip.hpp"

#include "src/thirdparty/miniz.h"
#include "src/core/logging.hpp"
#include <cstring>

bool Zip::unzip(
	const fs::path &zipFilePath,
	const fs::path &outputDir
) {
	mz_zip_archive zipFile;
	std::memset( &zipFile, 0, sizeof(mz_zip_archive) );

	if( !fs::exists( zipFilePath ) ) {
		logError( "Unzip failed because the zip file does not exist: "s + zipFilePath.u8string() );
		return false;
	}

	if( !mz_zip_reader_init_file( &zipFile, zipFilePath.u8string().c_str(), 0 ) ) {
		logError( "Unzip failed. Is this file actually a zip archive? "s + zipFilePath.u8string() );
		return false;
	}

	const mz_uint numFiles = mz_zip_reader_get_num_files( &zipFile );
	if( numFiles == 0 ) {
		logInfo( "Extracted empty zip file: "s + zipFilePath.u8string() );
		mz_zip_reader_end( &zipFile );
		return true;
	}

	mz_zip_archive_file_stat fileInfo;
	for( mz_uint i = 0; i < numFiles; i++ ) {
		if(
			!mz_zip_reader_file_stat( &zipFile, i, &fileInfo ) ||
			mz_zip_reader_is_file_a_directory( &zipFile, i )
		) continue;

		const fs::path extractPath = outputDir / fileInfo.m_filename;
		fs::create_directories( extractPath.parent_path() );

		if( !mz_zip_reader_extract_to_file( &zipFile, i, extractPath.u8string().c_str(), 0 ) ) {
			logError( "Unzip failed. Failed to extract '"s + extractPath.u8string() + "' from zip file: "s + zipFilePath.u8string() );
			mz_zip_reader_end( &zipFile );
			return false;
		}
	}

	mz_zip_reader_end( &zipFile );
	return true;
}

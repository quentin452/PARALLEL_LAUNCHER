#ifdef _WIN32

#include "src/core/sdcard.hpp"

#include <Windows.h>
#include <shellapi.h>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <thread>
#include "src/polyfill/windows/unicode.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/fifo-pipe.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/version-win.hpp"
#include "src/core/logging.hpp"
#include "src/core/uuid.hpp"
#include "src/core/time.hpp"
#include "src/core/zip.hpp"
#include "src/ui/download-dialog.hpp"

extern const ushort APPLICATION_MAJOR_VERSION;
extern const ushort APPLICATION_MINOR_VERSION;

static std::string getErrorMessage( DWORD errorCode ) {
	if( errorCode == 0 ) return "Unknown Error";

	const std::string prefix = Number::toString( (uint)errorCode ) + " - "s;
	char *buffer = nullptr;
	const size_t messageSize = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		errorCode,
		LANG_USER_DEFAULT,
		(char*)&buffer,
		0,
		nullptr
	);

	if( messageSize == 0 ) return prefix + "Unknown Error";
	std::string message( buffer, messageSize );

	if( buffer != nullptr ) LocalFree( buffer );
	return prefix + message;
}

static inline std::string getLastErrorMessage() {
	return getErrorMessage( GetLastError() );
}

static bool removeVhdFooter( const fs::path &filePath, const SdCardInfo &info, bool rename ) {
	const uintmax_t originalSize = (uintmax_t)info.numBlocks * 1024;

	HANDLE fileHandle = CreateFileW(
		filePath.wstring().c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if( !fileHandle ) {
		logWarn( "Failed to open SD Card image" );
		return false;
	}

	LARGE_INTEGER longFileSize;
	longFileSize.QuadPart = (uint64)originalSize;

	if( !SetFilePointerEx( fileHandle, longFileSize, nullptr, FILE_BEGIN ) ) {
		logWarn( "SD Card I/O Error: "s + getLastErrorMessage() );
		return false;
	}

	if( !SetEndOfFile( fileHandle ) ) {
		CloseHandle( fileHandle );
		logWarn( "Failed to remove VHD footer from SD Card image. SD Card image may now be corrupt." );
		return false;
	}

	// Work around a Windows bug that prevents disk images from being mounted if file compression is enabled
	ushort compressionMode = COMPRESSION_FORMAT_NONE;
	DWORD scratch;
	DeviceIoControl(
		fileHandle,
		FSCTL_SET_COMPRESSION,
		(void*)&compressionMode,
		sizeof( ushort ),
		nullptr,
		0,
		&scratch,
		nullptr
	);

	CloseHandle( fileHandle );

	if( rename ) {
		const fs::path newPath = filePath.parent_path() / (filePath.stem().wstring() + L".iso");

		fs::error_code err;
		fs::remove( newPath, err );

		err.clear();
		fs::rename( filePath, newPath, err );

		if( err ) {
			logError( "File operation error: "s + err.message() );
			return false;
		}
	}

	return true;
}

struct UnmountProcessInfo {
	SHELLEXECUTEINFOW shell;
	WritableFifoPipe procIn;
	ReadableFifoPipe procOut;
	std::wstring args;
	std::thread thread;
};

static void onHelperExit( UnmountProcessInfo *proc ) {
	if( !proc->shell.hProcess ) return;
	WaitForSingleObject( proc->shell.hProcess, INFINITE );
	proc->procIn.cancel();
	proc->procOut.cancel();
}

bool MountedSdCard::unmount() {
	if( !m_mounted ) return true;

	if( m_process ) {
		UnmountProcessInfo *proc = static_cast<UnmountProcessInfo*>( m_process );

		ubyte b = 0;
		std::error_code err;
		if(
			!proc->procIn.write( 1, &b, err ) ||
			!proc->procOut.read( 1, &b, err ) ||
			b != 0
		) return false;

		WaitForSingleObject( proc->shell.hProcess, INFINITE );
		proc->thread.join();
		CloseHandle( proc->shell.hProcess );

		delete proc;
		m_process = nullptr;
	}

	if( removeVhdFooter( m_device, m_info, true ) ) {
		m_mounted = false;
		return true;
	}

	return false;
}

MountedSdCard::~MountedSdCard() {
	if( m_process ) {
		UnmountProcessInfo *proc = static_cast<UnmountProcessInfo*>( m_process );

		ubyte b = 1;
		std::error_code err;
		proc->procIn.write( 1, &b, err );
		proc->procOut.read( 1, &b, err );

		WaitForSingleObject( proc->shell.hProcess, 500 );
		TerminateProcess( proc->shell.hProcess, 0 );
		proc->thread.join();
		CloseHandle( proc->shell.hProcess );

		delete proc;
		m_process = nullptr;
	}

	if( m_mounted ) {
		removeVhdFooter( m_device, m_info, true );
	}
}

static inline void writeLong( std::fstream &file, uintmax_t num ) {
	file.put( (char)(ubyte)(num >> 56) );
	file.put( (char)(ubyte)((num >> 48) & 0xFF) );
	file.put( (char)(ubyte)((num >> 40) & 0xFF) );
	file.put( (char)(ubyte)((num >> 32) & 0xFF) );
	file.put( (char)(ubyte)((num >> 24) & 0xFF) );
	file.put( (char)(ubyte)((num >> 16) & 0xFF) );
	file.put( (char)(ubyte)((num >> 8) & 0xFF) );
	file.put( (char)(ubyte)(num & 0xFF) );
}

static inline SdCardFormat getFilesystemFormat( std::fstream &image ) {
	char typeNameBuffer[5];
	image.seekg( 0x36, std::ios_base::beg );
	image.read( typeNameBuffer, 5 );
	
	if( !image.good() ) return SdCardFormat::None;
	if( std::memcmp( typeNameBuffer, "FAT12", 5 ) == 0 ) return SdCardFormat::FAT12;
	if( std::memcmp( typeNameBuffer, "FAT16", 5 ) == 0 ) return SdCardFormat::FAT16;

	image.seekg( 0x52, std::ios_base::beg );
	image.read( typeNameBuffer, 5 );

	if( image.good() && std::memcmp( typeNameBuffer, "FAT32", 5 ) == 0 ) return SdCardFormat::FAT32;
	return SdCardFormat::None;
}

static bool writeVhdFooter( fs::path &filePath, SdCardInfo &info ) {
	std::fstream image( filePath, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate );
	const auto footerStart = image.tellp();

	ubyte numHeads;
	ubyte sectorsPerTrack;

	image.seekg( 0x18, std::ios_base::beg );
	image.read( (char*)&sectorsPerTrack, 1 );
	image.seekg( 0x1A, std::ios_base::beg );
	image.read( (char*)&numHeads, 1 );

	if( !image.good() ) {
		logWarn( "SD Card I/O Error" );
		return false;
	}

	fs::error_code err;
	const uintmax_t fileSize = fs::file_size( filePath, err );
	if( err ) {
		logWarn( "Failed to get SD card image file size." );
		return false;
	}

	if( numHeads == 0 || sectorsPerTrack == 0 ) {
		const uintmax_t numSectors = fileSize / 512;

		if( numSectors <= 512 * 1024 ) {
			sectorsPerTrack = (numSectors <= 4096) ? 16 : 32;

			if( numSectors <= 32 * 1024 ) {
				numHeads = 2;
			} else if( numSectors <= 64 * 1024 ) {
				numHeads = 4;
			} else if( numSectors <= 256 * 1024 ) {
				numHeads = 8;
			} else {
				numHeads = 16;
			}
		} else {
			sectorsPerTrack = 63;

			if( numSectors <= 16 * 63 * 1024 ) {
				numHeads = 16;
			} else if( numSectors <= 32 * 16 * 1024 ) {
				numHeads = 32;
			} else if( numSectors <= 64 * 16 * 1024 ) {
				numHeads = 64;
			} else if( numSectors <= 128 * 16 * 1024 ) {
				numHeads = 128;
			} else {
				numHeads = 255;
			}
		}
	}

	const uintmax_t numCylinders = 1 + ((fileSize - 1) / (uintmax_t)512 / (uintmax_t)numHeads / (uintmax_t)sectorsPerTrack);
	if( numCylinders > (uintmax_t)0xFFFF ) {
		logWarn( "Sorry, but Windows cannot mount this type of disk if it is larger than 2032 GiB." );
		return false;
	}

	info.name = filePath.stem().u8string();
	info.format = getFilesystemFormat( image );
	info.numBlocks = (uint)(fileSize / 1024);

	image.seekg( 0x1BE, std::ios_base::beg );
	if( image.get() != (char)0x80 ) {
		// Write the partition table to the fake MBR because Windows needs it
		const uint numSectors = (uint)(fileSize / 512);

		if( info.format == SdCardFormat::None ) {
			logWarn( "SD card does not appear to have an FAT filesystem" );
			return false;
		}

		Uuid randomUuid = Uuid::Random();
		image.seekp( 0x1B8, std::ios_base::beg );
		image.write( (const char*)randomUuid.data(), 4 );
		
		image.seekp( 0x1BE, std::ios_base::beg );
		image.put( (char)0x80 );
		image.put( 0x00 );
		image.put( 0x01 );
		image.put( 0x00 );

		switch( info.format ) {
			case SdCardFormat::FAT12: image.put( 0x01 ); break;
			case SdCardFormat::FAT32: image.put( 0x0C ); break;
			default: image.put( (numSectors < (uint)numHeads * (uint)sectorsPerTrack * 1024) ? 0x06 : 0x0E ); break;
		}
		if( numSectors >= (uint)sectorsPerTrack * (uint)numHeads * 1024 ) {
			image.put( (char)0xFE );
			image.put( (char)0xFF );
			image.put( (char)0xFF );
		} else {
			image.put( (char)((ubyte)(numSectors / sectorsPerTrack) % numHeads) );
			image.put( (char)(ubyte)(((1 + (uint)numSectors % (uint)sectorsPerTrack) & 0x3F) | (((numSectors / ((uint)numHeads * (uint)sectorsPerTrack)) >> 8) << 6)) );
			image.put( (char)(ubyte)((numSectors / ((uint)numHeads * (uint)sectorsPerTrack)) & 0xFF) );
		}

		const uint zero = 0;
		image.write( (const char*)&zero, 4 );

		image.put( (char)(ubyte)(numSectors & 0xFF) );
		image.put( (char)(ubyte)((numSectors >> 8) & 0xFF) );
		image.put( (char)(ubyte)((numSectors >> 16) & 0xFF) );
		image.put( (char)(ubyte)(numSectors >> 24) );
	}

	image.seekp( footerStart );
	image.write( "conectix", 8 );
	
	const ubyte data1[] = {
		0x00, 0x00, 0x00, 0x02,
		0x00, 0x01, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};
	image.write( (const char*)data1, 16 );
	
	const auto lastModified = fs::last_write_time( filePath, err );
	const int64 time = (err ? Time::now() : Time::toUnixTime( lastModified )) - 946684800ull;

	image.put( (char)(ubyte)((time >> 24) & 0xFFull) );
	image.put( (char)(ubyte)((time >> 16) & 0xFFull) );
	image.put( (char)(ubyte)((time >> 8) & 0xFFull) );
	image.put( (char)(ubyte)(time & 0xFFull) );

	image.write( "pl  ", 4 );

	image.put( (char)(ubyte)(CurrentVersion_WIN32::major >> 8) );
	image.put( (char)(ubyte)(CurrentVersion_WIN32::major & 0xFF) );
	image.put( (char)(ubyte)(CurrentVersion_WIN32::minor >> 8) );
	image.put( (char)(ubyte)(CurrentVersion_WIN32::minor & 0xFF) );

	image.write( "Wi2k", 4 );

	writeLong( image, fileSize );
	writeLong( image, fileSize );

	image.put( (char)(ubyte)(numCylinders >> 8) );
	image.put( (char)(ubyte)(numCylinders & 0xff) );
	image.put( (char)numHeads );
	image.put( (char)sectorsPerTrack );

	image.write( (const char*)data1, 4 );

	const auto checksumStart = image.tellp();
	image.put( 0 ); image.put( 0 ); image.put( 0 ); image.put( 0 );

	const Uuid uuid = Uuid::Random();
	image.write( (const char*)uuid.data(), 16 );

	for( int i = 0; i < 428; i++ ) image.put( 0 );

	image.flush();

	image.seekg( footerStart );
	uint checksum = 0;
	for( int i = 0; i < 84; i++ ) {
		checksum += (uint)image.get();
	}
	checksum ^= 0xFFFFFFFFu;

	image.seekp( checksumStart );
	image.put( (char)(ubyte)(checksum >> 24) );
	image.put( (char)(ubyte)((checksum >> 16) & 0xFFu) );
	image.put( (char)(ubyte)((checksum >> 8) & 0xFFu) );
	image.put( (char)(ubyte)(checksum & 0xFFu) );

	image.flush();

	if( image.bad() ) {
		logWarn( "SD Card I/O Error" );
		removeVhdFooter( filePath, info, false );
		return false;
	}

	image.close();

	const fs::path newPath = filePath.parent_path() / (filePath.stem().wstring() + L".vhd");
	fs::remove( newPath, err );

	err.clear();
	fs::rename( filePath, newPath, err );

	if( err ) {
		logWarn( "File operation error: "s + err.message() );
		removeVhdFooter( filePath, info, false );
		return false;
	}

	filePath = newPath;
	return true;
}

static inline bool DownloadSdCardHelper( const fs::path &installDir ) {
	fs::error_code err;
	if(
		fs::is_regular_file( installDir / L"mount-sd-card.exe", err ) &&
		fs::is_regular_file( installDir / L"virtdisk.dll", err ) &&
		!err
	) return true;

	fs::remove_all( installDir, err );
	fs::create_directories( installDir, err );

	const fs::path zipPath = BaseDir::temp() / L"mount-sd-card.zip";
	fs::remove( zipPath, err );

	DownloadResult result = DownloadDialog::download(
		QT_TRANSLATE_NOOP( "DownloadDialog", "Downloading SD Card Manager Extension" ),
#ifdef _WIN64
		"https://parallel-launcher.ca/optional/windows_64/mount-sd-card.zip",
#else
		"https://parallel-launcher.ca/optional/windows_32/mount-sd-card.zip",
#endif
		zipPath
	);

	if( !result.success ) {
		logError( "Failed to download SD Card Manager extension: "s + result.errorMessage );
		return false;
	}

	if( !Zip::unzip( zipPath, installDir ) ) {
		logError( "Failed to extract the SD Card Manager extension" );
		fs::remove( zipPath, err );
		return false;
	}

	fs::remove( zipPath, err );
	return true;
}

std::shared_ptr<MountedSdCard> SdCardManager::mount( const std::string &name ) {
	fs::path imagePath = BaseDir::data() / L"sdcard" / fs::to_path(name + ".iso");

	SdCardInfo info;
	if( !writeVhdFooter( imagePath, info ) ) return nullptr;

#ifdef _WIN64
	const fs::path extensionDir = BaseDir::data().parent_path() / L"ext64";
#else
	const fs::path extensionDir = BaseDir::data().parent_path() / L"ext32";
#endif
	
	if( !DownloadSdCardHelper( extensionDir ) ) return nullptr;
	UnmountProcessInfo *proc = new UnmountProcessInfo();
	const std::string randomId = Uuid::Random().toString();

	std::error_code err;
	if( !proc->procIn.open( "sdmount-stdin-"s + randomId, err ) ) {
		logError( "Failed to create named pipe for writing: "s + err.message() );
		delete proc;
		removeVhdFooter( imagePath, info, true );
		return nullptr;
	}

	if( !proc->procOut.open( "sdmount-stdout-"s + randomId, err ) ) {
		logError( "Failed to create named pipe for reading: "s + err.message() );
		delete proc;
		removeVhdFooter( imagePath, info, true );
		return nullptr;
	}

	static const std::wstring procPath = (extensionDir / L"mount-sd-card.exe").wstring();
	proc->args = Unicode::toUtf16( proc->procIn.filePath() + ' ' + proc->procOut.filePath() );
	proc->shell.cbSize = sizeof(SHELLEXECUTEINFOW);
	proc->shell.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
	proc->shell.hwnd = nullptr;
	proc->shell.lpVerb = L"runas";
	proc->shell.lpFile = procPath.c_str();
	proc->shell.lpParameters = proc->args.c_str();
	proc->shell.lpDirectory = nullptr;
	proc->shell.nShow = SW_HIDE;
	proc->shell.hInstApp = nullptr;

	if( !ShellExecuteExW( &proc->shell ) || !proc->shell.hProcess ) {
		logWarn( "Failed to launch mount-sd-card.exe helper program: "s + getLastErrorMessage() );
		delete proc;
		removeVhdFooter( imagePath, info, true );
		return nullptr;
	}

	proc->thread = std::thread( onHelperExit, proc );

	const ushort pathLen = (ushort)imagePath.wstring().length();
	const ushort pathLenBE = _byteswap_ushort( pathLen );
	ubyte status;
	if(
		!proc->procOut.connect( err ) ||
		!proc->procIn.connect( err ) ||
		!proc->procIn.write( sizeof(ushort), &pathLenBE, err ) ||
		!proc->procIn.write( (uintmax_t)pathLen * sizeof(wchar_t), imagePath.wstring().c_str(), err ) ||
		!proc->procOut.read( 1, &status, err )
	) {
		logWarn( "I/O error communicating with mount-sd-card.exe helper program: "s + err.message() );
		proc->procIn.cancel();
		proc->procOut.cancel();
		TerminateProcess( proc->shell.hProcess, 0 );
		proc->thread.join();
		CloseHandle( proc->shell.hProcess );
		delete proc;
		removeVhdFooter( imagePath, info, true );
		return nullptr;
	}

	if( status != 0 ) {
		DWORD errorCode = 0;
		if( status == 1 || status == 2 ) {
			proc->procOut.read( sizeof(DWORD), &errorCode, err );
#if __BYTE_ORDER == __LITTLE_ENDIAN
			errorCode = _byteswap_ulong( errorCode );
#endif
		}

		string errorMsg;
		switch( status ) {
			case 1: errorMsg = "Failed to open virtual SD card: "s + getErrorMessage( errorCode ); break;
			case 2: errorMsg = "Failed to mount virtual SD card: "s + getErrorMessage( errorCode ); break;
			case 3: errorMsg = "Windows either could not find any volumes on the SD card image or it failed to assign a drive letter."; break;
			default: errorMsg = "I/O error communicating with mount-sd-card.exe helper program."; break;
		}

		logWarn( errorMsg );
		proc->procIn.cancel();
		proc->procOut.cancel();
		TerminateProcess( proc->shell.hProcess, 0 );
		proc->thread.join();
		CloseHandle( proc->shell.hProcess );
		delete proc;
		removeVhdFooter( imagePath, info, true );
		return nullptr;
	}

	char driveLetter;
	if( !proc->procOut.read( 1, &driveLetter, err )) {
		logWarn( "Failed to assign a drive letter to the SD card volume." );
		proc->procIn.cancel();
		proc->procOut.cancel();
		TerminateProcess( proc->shell.hProcess, 0 );
		proc->thread.join();
		CloseHandle( proc->shell.hProcess );
		delete proc;
		removeVhdFooter( imagePath, info, true );
		return nullptr;
	}

	return std::make_shared<MountedSdCard>(
		""s + driveLetter + ":",
		imagePath.u8string(),
		proc,
		std::move( info )
	);
}

static inline int64 getNumericSuffix( const string &name, size_t &baseLength ) {
	size_t start = name.find_last_of( '~' );
	if( start == string::npos ) return 0;

	size_t end = name.find_last_not_of( ' ' );
	if( end == string::npos ) return 0;

	if( start++ >= end++ ) return 0;
	if( end - start > 1 && name[start] == '0' ) return 0;

	for( size_t i = start; i < end; i++ ) {
		if( (ubyte)name[i] < (ubyte)'0' || (ubyte)name[i] > (ubyte)'9' ) return 0;
	}

	baseLength = start - 1;
	return Number::parseInt64( name.substr( start, end - start ) );
}

void SdCardManager::recoverSdCards() {
	fs::error_code err;
	for( const fs::directory_entry &node : fs::directory_iterator( BaseDir::data() / L"sdcard", err ) ) {
		err.clear();
		if( !node.is_regular_file( err ) || err ) continue;
		if( node.path().extension().u8string() != ".vhd" ) continue;

		SdCardInfo info;
		info.numBlocks = (uint)(fs::file_size( node.path(), err ) / 1024ull);
		info.name = node.path().stem().u8string();
		if( err ) continue;

		if( !removeVhdFooter( node.path(), info, false ) ) continue;

		err.clear();
		while( !err ) {
			const fs::path target = BaseDir::data() / L"sdcard" / fs::to_path(info.name + ".iso");
			if( !fs::exists( target, err ) && !err ) {
				fs::rename( node.path(), target, err );
				break;
			}

			size_t baseLength = info.name.length();
			const string suffix = "~"s + Number::toString( getNumericSuffix( info.name, baseLength ) + 1 );
			info.name = info.name.substr( 0, min( baseLength, (size_t)11 - suffix.length() ) ) + suffix;
		}
	}
}

#endif

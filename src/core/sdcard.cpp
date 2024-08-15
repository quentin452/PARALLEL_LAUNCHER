#include "src/core/sdcard.hpp"

#include "src/core/numeric-string.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file.hpp"
#include <cstring>

static SdCardFormat getCardFormat( InputFile &file ) {
	char typeNameBuffer[5];

	try {
		file.seekg( 0x36, std::ios_base::beg );
		file.read( typeNameBuffer, 5 );
		
		if( !file.good() ) return SdCardFormat::None;
		if( std::memcmp( typeNameBuffer, "FAT12", 5 ) == 0 ) return SdCardFormat::FAT12;
		if( std::memcmp( typeNameBuffer, "FAT16", 5 ) == 0 ) return SdCardFormat::FAT16;

		file.seekg( 0x52, std::ios_base::beg );
		file.read( typeNameBuffer, 5 );

		if( file.good() && std::memcmp( typeNameBuffer, "FAT32", 5 ) == 0 ) return SdCardFormat::FAT32;
	} catch( ... ) {}

	return SdCardFormat::None;
}

std::vector<SdCardInfo> SdCardManager::fetchAll() {
	const fs::path directory = BaseDir::data() / _NFS("sdcard");
	std::vector<SdCardInfo> cards;

	if( !fs::is_directory( directory ) ) {
		return cards;
	}

	for( auto const &inode : fs::directory_iterator( directory ) ) {
		fs::error_code err;

		if( !fs::is_regular_file( inode.path(), err ) || err ) continue;
		if( inode.path().extension().string() != ".iso" ) continue;
		const uintmax_t fileSize = fs::file_size( inode.path(), err );
		if( err || fileSize > 2199023255552ull ) continue;

		InputFile file( inode.path(), true );
		cards.push_back({
			inode.path().stem().u8string(),
			getCardFormat( file ),
			(uint)(fileSize / 1024)
		});
	}

	return cards;
}

bool SdCardManager::tryFetch( const std::string &name, SdCardInfo &info ) {
	const fs::path filePath = BaseDir::data() / _NFS("sdcard") / fs::to_path(name + ".iso");

	fs::error_code err;
	if( !fs::is_regular_file( filePath, err ) || err ) return false;

	const uintmax_t fileSize = fs::file_size( filePath, err );
	if( err || fileSize > 2199023255552ull ) return false;

	InputFile file( filePath, true );
	info.name = name;
	info.format = getCardFormat( file );
	info.numBlocks = (uint)(fileSize / 1024);
	return true;
}

bool SdCardManager::deleteCard( const std::string &name ) {
	const fs::path filePath = BaseDir::data() / _NFS("sdcard") / fs::to_path(name + ".iso");
	fs::error_code err;
	fs::remove( filePath, err );
	return !err;
}

AsyncProcess SdCardManager::createCardAsync( const SdCardInfo &info ) {
	int fatType = 0;
	switch( info.format ) {
		case SdCardFormat::FAT12: fatType = 12; break;
		case SdCardFormat::FAT16: fatType = 16; break;
		case SdCardFormat::FAT32: fatType = 32; break;
		default: fatType = 0;
	}

	fs::error_code err;
	fs::create_directories( BaseDir::data() / _NFS("sdcard"), err );

	const fs::path filePath = BaseDir::data() / _NFS("sdcard") / fs::to_path(info.name + ".iso");
	if( fatType == 0 ) {
		// Create a sparse file
#ifdef _WIN32
		OutputFile( filePath, true ).close();
		AsyncProcess makeSparseCmd( "fsutil", { "sparse", "setflag", filePath.u8string(), "1" } );
		if( makeSparseCmd.join() != 0 ) return makeSparseCmd;
		return AsyncProcess( "fsutil", { "file", "seteof", filePath.u8string(), Number::toString( info.numBlocks * 1024 ) } );
#else
		return AsyncProcess( "dd", { "if=/dev/zero", "of="s + filePath.u8string(), "bs=1", "count=0", "seek="s + Number::toString( info.numBlocks ) + "k" } );
#endif
	}

	std::vector<string> args = { "-C", "-F", Number::toString( fatType ), "-n", info.name, "-S", "512" };
	args.push_back( filePath.u8string() );
#ifdef _WIN32
	args.push_back( Number::toString( 2 * info.numBlocks ) );
#else
	args.push_back( Number::toString( info.numBlocks ) );
#endif

#if defined(_WIN32)
	return AsyncProcess( (BaseDir::program() / L"mkdosfs.exe").u8string(), args );
#elif defined(__APPLE__)
	return AsyncProcess( (BaseDir::program().parent_path() / "Resources" / "mkfs.fat").u8string(), args );
#else
	return AsyncProcess( "mkfs.fat", args );
#endif
}

bool SdCardManager::copyCard( const std::string &source, const std::string &target ) {
	fs::error_code err;
	return fs::copy_file(
		BaseDir::data() / _NFS("sdcard") / fs::to_path(source + ".iso"),
		BaseDir::data() / _NFS("sdcard") / fs::to_path(target + ".iso"),
		err
	) && !err;
}

bool SdCardManager::canHardLink( const fs::path &filePath ) {
	const fs::path tempFile = BaseDir::data() / _NFS("sdcard") / _NFS(".ln");
	fs::error_code discard, err;

	fs::remove( tempFile, discard );
	fs::create_hard_link( filePath, tempFile, err );
	fs::remove( tempFile, discard );

	return !err;
}

std::string SdCardManager::tryReadVolumeName( const fs::path &filePath ) {
	char name[12];
	std::memset( name, 0, 12 );

	InputFile file( filePath, true );
	switch( getCardFormat( file ) ) {
		case SdCardFormat::FAT12:
		case SdCardFormat::FAT16:
			file.seekg( 0x2B, std::ios_base::beg );
			file.read( name, 11 );
			break;
		case SdCardFormat::FAT32:
			file.seekg( 0x47, std::ios_base::beg );
			file.read( name, 11 );
			break;
		default:
			return std::string();
	}

	if( !file.good() ) return std::string();

	// trim trailing whitespace
	for( int i = 10; i >= 0 && name[i] == ' '; i-- ) {
		name[i] = '\0';
	}

	// check for invalid characters
	for( int i = 0; name[i] != '\0'; i++ ) {
		const char c = name[i];
		if( (ubyte)c < 32 || (ubyte)c > 126 || c == '<' || c == '>' || c == ':' || c == '"' || c == '/' || c == '\\' || c == '|' || c == '?' || c == '*' ) return std::string();
	}

	return std::string( name );
}

bool SdCardManager::import( const fs::path &filePath, const std::string &name, bool copy ) {
	const fs::path dest = BaseDir::data() / _NFS("sdcard") / fs::to_path(name + ".iso");
	fs::error_code err;

	if( copy ) {
		fs::copy_file( filePath, dest, err );
	} else {
		fs::create_hard_link( filePath, dest, err );
	}

	return !err;
}

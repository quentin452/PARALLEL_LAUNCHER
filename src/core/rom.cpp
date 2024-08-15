#include "src/core/rom.hpp"

#include <unordered_map>
#include <cstdint>
#include <utility>
#include "src/polyfill/file-search.hpp"
#include "src/polyfill/file.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/buffer.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/time.hpp"
#include "src/core/logging.hpp"
#include "src/core/utf8.hpp"

const N64InputTypes UNCHECKED_INPUT_TYPES = { (N64InputType)0xDE, (N64InputType)0xAD, (N64InputType)0xBE, (N64InputType)0xEF };

static constexpr char P_GROUPS[] = "tags";
static constexpr char P_FOLDER[] = "folder_path";
static constexpr char P_RECURSIVE[] = "recursive";
static constexpr char P_IGNORE_HIDDEN[] = "ignore_hidden";
static constexpr char P_FOLLOW_SYMLINKS[] = "follow_symlinks";
static constexpr char P_MAX_DEPTH[] = "max_depth";

template<> void JsonSerializer::serialize<RomSource>( JsonWriter &jw, const RomSource &obj ) {
	jw.writeObjectStart();
	jw.writeProperty( P_FOLDER, obj.folder.u8string() );
	jw.writeProperty( P_RECURSIVE, obj.recursive );
	jw.writeProperty( P_IGNORE_HIDDEN, obj.ignoreHidden && obj.recursive );
	jw.writeProperty( P_FOLLOW_SYMLINKS, obj.followSymlinks && obj.recursive );
	jw.writeProperty( P_MAX_DEPTH, obj.recursive ? obj.maxDepth : 5 );
	jw.writePropertyName( P_GROUPS );
	jw.writeArrayStart();
	for( const string &tag : obj.autoTags ) {
		jw.writeString( tag );
	}
	jw.writeArrayEnd();
	jw.writeObjectEnd();
}

template<> RomSource JsonSerializer::parse<RomSource>( const Json &json ) {
	std::set<string> tags;
	for( const Json &tag : json[P_GROUPS].array() ) {
		tags.insert( tag.get<string>() );
	}

	return RomSource{
		fs::to_path( json[P_FOLDER].get<string>() ),
		json[P_RECURSIVE].get<bool>(),
		json[P_IGNORE_HIDDEN].get<bool>(),
		json[P_FOLLOW_SYMLINKS].get<bool>(),
		json[P_MAX_DEPTH].get<ubyte>(),
		std::move( tags )
	};
}

template<> void JsonSerializer::serialize<fs::path>( JsonWriter &jw, const fs::path &obj ) {
	jw.writeString( obj.u8string() );
}

template<> fs::path JsonSerializer::parse<fs::path>( const Json &json ) {
	return fs::to_path( json.get<string>() );
}

static inline void unscrambleN64( ubyte *name ) {
	for( int i = 0; i < 20; i+= 4 ) {
		std::swap( name[i], name[i+3] );
		std::swap( name[i+1], name[i+2] );
	}
}

static inline void unscrambleV64( ubyte *name ) {
	for( int i = 0; i < 20; i+= 2 ) {
		std::swap( name[i], name[i+1] );
	}
}

static const ushort s_kanaTable[64] = {
	0x00A0, 0x8082, 0x808C, 0x808D, 0x8081, 0x83BB, 0x83B2, 0x82A1, 0x82A3, 0x82A5, 0x82A7, 0x82A9, 0x83A3, 0x83A5, 0x83A7, 0x8383,
	0x83BC, 0x82A2, 0x82A4, 0x82A6, 0x82A8, 0x82AA, 0x82AB, 0x82AD, 0x82AF, 0x82B1, 0x82B3, 0x82B5, 0x82B7, 0x82B9, 0x82BB, 0x82BD,
	0x82BF, 0x8381, 0x8384, 0x8386, 0x8388, 0x838A, 0x838B, 0x838C, 0x838D, 0x838E, 0x838F, 0x8392, 0x8395, 0x8398, 0x839B, 0x839E,
	0x839F, 0x83A0, 0x83A1, 0x83A2, 0x83A4, 0x83A6, 0x83A8, 0x83A9, 0x83AA, 0x83AB, 0x83AC, 0x83AD, 0x83AF, 0x83B3, 0x829B, 0x829C
};

string RomUtil::getInternalName( const fs::path romPath ) {
	ubyte sjsName[20];
	InputFile romFile( romPath, true );

	romFile.seekg( 0 );
	const ubyte firstByte = (ubyte)romFile.get();

	romFile.seekg( 0x20 );
	romFile.read( (char*)sjsName, 20 );
	if( !romFile.good() ) return string();
	romFile.close();

	if( firstByte == 0x40 ) {
		unscrambleN64( sjsName );
	} else if( firstByte == 0x37 ) {
		unscrambleV64( sjsName );
	}

	bool invalid = false;

	string name;
	for( int i = 0; i < 20; i++ ) {
		const ubyte c = sjsName[i];

		if( c == 0x5C ) {
			name += (char)0xC2;
			name += (char)0xA5;
		} else if( c == 0x7E ) {
			name += (char)0xE2;
			name += (char)0x80;
			name += (char)0xBE;
		} else if( c == 0xDE && i != 0 && (ubyte)sjsName[i-1] > 0xA0 && (ubyte)sjsName[i-1] < 0xDE ) {
			name += (char)0xE3;
			name += (char)0x82;
			name += (char)0x99;
		} else if( c == 0xDF && i != 0 && (ubyte)sjsName[i-1] > 0xA0 && (ubyte)sjsName[i-1] < 0xDE ) {
			name += (char)0xE3;
			name += (char)0x82;
			name += (char)0x9A;
		} else if( c > 0xA0 && c < 0xE0 ) {
			const ushort wc = s_kanaTable[c - 0xA0];
			name += (char)0xE3;
			name += (char)(wc >> 8);
			name += (char)(wc & 0xFF);
		} else if( c == 0 ) {
			name += ' ';
		} else if( c > 0x7F ) {
			invalid = true;
		} else {
			name += (char)c;
		}
	}

	if( invalid && Utf8::validate( (const char*)sjsName ) ) {
		name = (const char*)sjsName;
	}

	int n = (int)name.length();
	while( n > 0 && name[--n] == ' ' );

	return name.substr( 0, n + 1 );
}

int64 RomUtil::getLastModified( const fs::path romPath ) {
	fs::error_code err;
#ifdef __APPLE__
	return (int64)fs::last_write_time( romPath, err );
#else
	return Time::toUnixTime( fs::last_write_time( romPath, err ) );
#endif
}

static const uint s_crcTable[16] = {
	0x00000000, 0x1DB71064, 0x3B6E20C8, 0x26D930AC, 0x76DC4190, 0x6B6B51F4, 0x4DB26158, 0x5005713C,
	0xEDB88320, 0xF00F9344, 0xD6D6A3E8, 0xCB61B38C, 0x9B64C2B0, 0x86D3D2D4, 0xA00AE278, 0xBDBDF21C,
};

uint RomUtil::getCrc32( const fs::path &romPath ) {
	InputFile romFile( romPath, true );
	uint crc = 0xFFFFFFFFu;

	while( true ) {
		const ubyte x = (ubyte)romFile.get();
		if( romFile.eof() ) return ~crc;

		if( !romFile.good() ) {
			logWarn( "Failed to compute CRC32 checksum for rom "s + romPath.u8string() );
			return 0xDEADBEEFu;
		}

		crc = s_crcTable[(crc ^ x) & 0x0F] ^ (crc >> 4);
		crc = s_crcTable[(crc ^ (x>>4)) & 0x0F] ^ (crc >> 4);
	}
}

bool RomUtil::coveredBySearchPath( const fs::path &romPath ) {
	const bool isHidden = romPath.stem().u8string()[0] == '.';

	const std::vector<RomSource> sources = FileController::loadRomSources();
	for( const RomSource &source : sources ) {
		if( isHidden && source.ignoreHidden ) continue;

		if( source.recursive ) {
			fs::path aPath = romPath.parent_path();
			for( ubyte i = 0; i < source.maxDepth && !aPath.empty(); i++ ) {
				if( aPath == source.folder ) return true;

				if( source.ignoreHidden && aPath.stem().u8string()[0] == '.' ) break;
				if( !source.followSymlinks && fs::isSymlinkSafe( aPath ) ) break;

				aPath = aPath.parent_path();
			}
		} else if( romPath.parent_path() == source.folder ) {
			return true;
		}
	}

	return false;
}

N64InputTypes RomUtil::readControllerTypes( const fs::path &romPath ) {
	InputFile romFile( romPath, true );

	N64InputTypes controllerTypes;
	controllerTypes.fill( N64InputType::Unknown );

	romFile.seekg( 0 );
	const ubyte firstByte = (ubyte)romFile.get();

	ubyte romHeader[0x40];
	romFile.seekg( 0 );
	romFile.read( (char*)romHeader, 0x40 );
	if( !romFile.good() ) return UNCHECKED_INPUT_TYPES;

	// Convert to big endian
	if( firstByte == 0x40 ) {
		for( size_t i = 0; i < 0x40; i += 4 ) {
			std::swap( romHeader[i], romHeader[i+3] );
			std::swap( romHeader[i+1], romHeader[i+2] );
		}
	} else if( firstByte == 0x37 ) {
		for( size_t i = 0; i < 0x40; i += 2 ) {
			std::swap( romHeader[i], romHeader[i+1] );
		}
	}

	if( romHeader[0x3C] == (ubyte)'E' && romHeader[0x3D] == (ubyte)'D' ) {
		controllerTypes[0] = (N64InputType)romHeader[0x34];
		controllerTypes[1] = (N64InputType)romHeader[0x35];
		controllerTypes[2] = (N64InputType)romHeader[0x36];
		controllerTypes[3] = (N64InputType)romHeader[0x37];
	}

	return controllerTypes;
}

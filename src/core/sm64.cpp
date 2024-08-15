#include "src/core/sm64.hpp"

#include <cstring>
#include <QtGlobal>
#include "src/db/data-provider.hpp"

using namespace SM64;

const char *const SM64::CourseNames[25] = {
	"Bob-Omb Battlefield",
	"Whomp's Fortress",
	"Jolly Rodger Bay",
	"Cool Cool Mountain",
	"Big Boo's Haunt",
	"Hazy Maze Cave",
	"Lethal Lava Land",
	"Shifting Sand Land",
	"Dire Dire Docks",
	"Snowman's Land",
	"Wet Dry World",
	"Tall, Tall Mountain",
	"Tiny-Huge Island",
	"Tick Tock Clock",
	"Rainbow Ride",

	"Bowser in the Dark World",
	"Bowser in the Fire Sea",
	"Bowser in the Sky",
	"Peach's Secret Slide",
	"Cavern of the Metal Cap",
	"Tower of the Wing Cap",
	"Vanish Cap Under the Moat",
	"Winged Mario over the Rainbow",
	"Secret Aquarium",

	"End Screen"
};

const char *const SM64::LevelNames[37] = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Big Boo's Haunt" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Cool Cool Mountain" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Castle Interior" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Hazy Maze Cave" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Shifting Sand Land" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bob-Omb Battlefield" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Snowman's Land" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Wet Dry World" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Jolly Rodger Bay" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Tiny-Huge Island" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Tick Tock Clock" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Rainbow Ride" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Castle Grounds" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bowser in the Dark World" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Vanish Cap Under the Moat" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bowser in the Fire Sea" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Secret Aquarium" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bowser in the Sky" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Lethal Lava Land" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Dire Dire Docks" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Whomp's Fortress" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "End Screen" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Castle Courtyard" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Peach's Secret Slide" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Cavern of the Metal Cap" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Tower of the Wing Cap" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bowser 1" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Winged Mario over the Rainbow" ),
	nullptr,
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bowser 2" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bowser 3" ),
	nullptr,
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Tall, Tall Mountain" )
};

const std::array<LevelId,31> SM64::LevelIds = {
	LevelId::CastleGrounds,
	LevelId::CastleInterior,
	LevelId::CastleCourtyard,
	LevelId::BobOmbBattlefield,
	LevelId::WhompsFortress,
	LevelId::JollyRodgerBay,
	LevelId::CoolCoolMountain,
	LevelId::BigBoosHaunt,
	LevelId::HazyMazeCave,
	LevelId::LethalLavaLand,
	LevelId::ShiftingSandLand,
	LevelId::DireDireDocks,
	LevelId::SnowmansLands,
	LevelId::WetDryWorld,
	LevelId::TallTallMountain,
	LevelId::TinyHugeIsland,
	LevelId::TickTockClock,
	LevelId::RainbowRide,
	LevelId::BowserInTheDarkWorld,
	LevelId::BowserInTheFireSea,
	LevelId::BowserInTheSky,
	LevelId::Bowser1,
	LevelId::Bowser2,
	LevelId::Bowser3,
	LevelId::TowerOfTheWingCap,
	LevelId::CavernOfTheMetalCap,
	LevelId::VanishCapUnderTheMoat,
	LevelId::PeachsSecretSlide,
	LevelId::SecretAquarium,
	LevelId::WingedMarioOverTheRainbow,
	LevelId::EndScreen
};

struct Checksum {
	ubyte upper;
	ubyte lower;
};

static Checksum getChecksum( const ubyte *data ) noexcept {
	ushort checksum = 0;
	for( sbyte i = 0; i < 54; i++ ) {
		checksum += data[i];
	}
	return Checksum{
		(ubyte)(checksum >> 8),
		(ubyte)(checksum & 0xFF)
	};
}

static inline void writeSaveSlot( std::ostream &saveFile, const SaveSlot &slot ) {
	ubyte data[56];

	data[0] = (ubyte)slot.capLevel;
	data[1] = slot.capArea & 0x7;

	std::memset( &data[2], 0, 6 );

	data[8] = slot.castleStars & 0x7F;
	data[9] = (ubyte)(((uint)slot.flags >> 16) & 0x1F);
	data[10] = (ubyte)(((uint)slot.flags >> 8) & 0xFF);
	data[11] = (ubyte)((uint)slot.flags & 0xFF);

	// Due to a programming error, the bit controlling whether or not the cannon
	// is open is actually stored in the NEXT course's byte.
	data[12] = slot.starFlags[0] & 0x7F;
	for( sbyte i = 1; i < 25; i++ ) {
		data[i + 12] = (slot.starFlags[i] & 0x7F) | (slot.starFlags[i-1] & 0x80);
	}

	std::memcpy( &data[37], slot.coinScores, 15 );

	data[52] = 0x44;
	data[53] = 0x41;

	const Checksum checksum = getChecksum( data );
	data[54] = checksum.upper;
	data[55] = checksum.lower;

	saveFile.write( (const char*)data, 56 );
	saveFile.write( (const char*)data, 56 );
}

static inline void readSaveSlot( std::istream &saveFile, SaveSlot &slot ) {
	ubyte data[56];
	Checksum checksum;

	saveFile.read( (char*)data, 56 );
	checksum = getChecksum( data );

	if( checksum.upper == data[54] && checksum.lower == data[55] ) {
		saveFile.seekg( 56, std::ios_base::cur );
	} else {
		saveFile.read( (char*)data, 56 );
		checksum = getChecksum( data );

		if( checksum.upper != data[54] || checksum.lower != data[55] ) {
			std::memset( data, 0, 56 );
		}
	}

	slot.capLevel = (LevelId)data[0];
	slot.capArea = data[1] & 0x7;

	slot.castleStars = data[8] & 0x7F;
	slot.flags = (SaveFileFlag)(
		((uint)(data[9] & 0x1F) << 16) |
		((uint)data[10] << 8 ) |
		(uint)data[11]
	);

	for( sbyte i = 0; i < 25; i++ ) {
		// off-by-one error in reading the cannon state is intentional
		slot.starFlags[i] = (data[12 + i] & 0x7F) | (data[13 + i] & 0x80);
	}

	std::memcpy( slot.coinScores, &data[37], 15 );
}

void SaveFile::write( std::ostream &saveFile ) const {
	saveFile.seekp( 0 );
	for( sbyte i = 0; i < 4; i++ ) {
		writeSaveSlot( saveFile, m_slots[i] );
	}
}

SaveFile SaveFile::read( std::istream &saveFile ) {
	saveFile.seekg( 0 );

	SaveFile saveData;
	for( sbyte i = 0; i < 4; i++ ) {
		readSaveSlot( saveFile, saveData.m_slots[i] );
	}

	return saveData;
}

static inline int bitcount( uint flags ) {
#ifdef _WIN32
	int count = 0;
	while( flags != 0 ) {
		flags &= flags - 1;
		count++;
	}
	return count;
#else
	return __builtin_popcount( flags );
#endif
}

int SaveSlot::countStars() const {
	int starCount = 0;
	uint *wStarFlags = (uint*)starFlags;

	for( int i = 0; i < 6; i++ ) {
		starCount += bitcount( wStarFlags[i] & 0x7F7F7F7Fu );
	}

	starCount += bitcount( (uint)starFlags[24] & 0x7Fu );
	starCount += bitcount( (uint)castleStars & 0x7Fu );

	return starCount;
}

bool SM64::tryGetPath( fs::path &path ) {
	return DataProvider::tryFetchRomPathWithSha1( "9bef1128717f958171a4afac3ed78ee2bb4e86ce", path );
}

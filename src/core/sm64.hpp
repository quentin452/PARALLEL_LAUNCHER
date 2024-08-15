#ifndef SRC_CORE_SM64_HPP_
#define SRC_CORE_SM64_HPP_

#include <array>
#include "src/types.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/flags.hpp"

namespace SM64 {

	extern const char *const CourseNames[25];
	extern const char *const LevelNames[37];

	enum class LevelId : ubyte {
		Null = 0,

		CastleGrounds = 16,
		CastleInterior = 6,
		CastleCourtyard = 26,

		BobOmbBattlefield = 9,
		WhompsFortress = 24,
		JollyRodgerBay = 12,
		CoolCoolMountain = 5,
		BigBoosHaunt = 4,
		HazyMazeCave = 7,
		LethalLavaLand = 22,
		ShiftingSandLand = 8,
		DireDireDocks = 23,
		SnowmansLands = 10,
		WetDryWorld = 11,
		TallTallMountain = 36,
		TinyHugeIsland = 13,
		TickTockClock = 14,
		RainbowRide = 15,

		BowserInTheDarkWorld = 17,
		BowserInTheFireSea = 19,
		BowserInTheSky = 21,
		Bowser1 = 30,
		Bowser2 = 33,
		Bowser3 = 34,

		TowerOfTheWingCap = 29,
		CavernOfTheMetalCap = 28,
		VanishCapUnderTheMoat = 18,

		PeachsSecretSlide = 27,
		SecretAquarium = 20,
		WingedMarioOverTheRainbow = 31,

		EndScreen = 25
	};

	extern const std::array<LevelId,31> LevelIds;

	enum class SaveFileFlag : uint {
		Exists = 0x000001,
		HasWingCap = 0x000002,
		HasMetalCap = 0x000004,
		HasVanishCap = 0x000008,
		HasBasementKey = 0x000010,
		HasUpstairsKey = 0x000020,
		UnlockedBasement = 0x000040,
		UnlockedUpstairs = 0x000080,
		BowserSubGone = 0x000100,
		MoatDrained = 0x000200,
		UnlockedSlideDoor = 0x000400,
		UnlockedWhompsDoor = 0x000800,
		UnlockedCCMDoor = 0x001000,
		UnlockedJRBDoor = 0x002000,
		UnlockedDarkWorldDoor = 0x004000,
		UnlockedFireSeaDoor = 0x008000,
		CapOnGround = 0x010000,
		CapOnKlepto = 0x020000,
		CapOnUkiki = 0x040000,
		CapOnSnowman = 0x080000,
		UnlockedTippyDoor = 0x100000
	};

	DEFINE_FLAG_OPERATIONS( SaveFileFlag, uint )

	struct SaveSlot {
		LevelId capLevel;
		ubyte capArea;

		SaveFileFlag flags;
		ubyte castleStars;

		ubyte starFlags[25];
		ubyte coinScores[15];

		int countStars() const;
	};

	class SaveFile {

		private:
		SaveSlot m_slots[4];

		public:
		void write( std::ostream &saveFile ) const;
		static SaveFile read( std::istream &saveFile );

		inline const SaveSlot &slot( int i ) const { return m_slots[i]; }
		inline SaveSlot &slot( int i ) { return m_slots[i]; }

	};

	extern bool tryGetPath( fs::path &path );

}

/*
Castle Stars:
0 - Toad 1
1 - Toad 2
2 - Toad 3
3 - MIPS 1
4 - MIPS 2
5 - unused
6 - unused
7 - unused
 */


#endif /* SRC_CORE_SM64_HPP_ */

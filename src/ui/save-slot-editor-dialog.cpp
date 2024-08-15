#include "src/ui/save-slot-editor-dialog.hpp"
#include "ui_save-slot-editor-dialog.h"

#include <cstring>
#include <QHBoxLayout>
#include "src/polyfill/file.hpp"
#include "src/core/sm64.hpp"
#include "src/ui/util.hpp"
#include "src/ui/icons.hpp"

static const char *REDS = QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "8 Red Coins" );
static const char *COINS = QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "100 Coins" );

static const char *s_starNames[26][7] = {
	/* Castle */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Toad 1" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Toad 2" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Toad 3" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "MIPS 1" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "MIPS 2" ),
		nullptr,
		nullptr
	},
	/* Bob-Omb's Battlefield */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Big Bob-Omb on the Summit" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Footface with Koopa the Quick" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Shoot to the Island in the Sky" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Find the 8 Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Mario Wings to the Sky" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Behind Chain Chomp's Gate" ),
		COINS
	},
	/* Whomp's Fortress */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Chip Off Whomp's Block" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "To the Top of the Fortress" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Shoot into the Wild Blue" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Red Coins on the Floating Isle" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Fall onto the Caged Island" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Blast Away the Wall" ),
		COINS
	},
	/* Jolly Rodger Bay */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Plunder in the Sunken Ship" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Can the Eel Come Out to Play?" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Treasure of the Ocean Cave" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Red Coins on the Ship Afloat" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Blast to the Stone Pillar" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Through the Jet Stream" ),
		COINS
	},
	/* Cool Cool Mountain */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Slip Slidin' Away" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Li'l Penguin Lost" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Big Penguin Race" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Frosty Slide for 8 Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Snowman's Lost his Head" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Wall Kicks Will Work" ),
		COINS
	},
	/* Big Boo's Haunt */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Go on a Ghost Hunt" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Ride Big Boo's Merry-Go-Round" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Secret of the Haunted Books" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Seek the 8 Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Big Boo's Balcony" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Eye to Eye in the Secret Room" ),
		COINS
	},
	/* Hazy Maze Cave */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Swimming Beast in the Cavern" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Elevate for 8 Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Metal-Head Mario Can Move!" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Navigating the Toxic Maze" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "A-maze-ing Emergency Exit" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Watch for Rolling Rocks" ),
		COINS
	},
	/* Lethal Lava Land */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Boil the Big Bully" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Bully the Bullies" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "8-Coin Puzzle with 15 Pieces" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Red-Hot Log Rolling" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Hot-Foot-it into the Volcano" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Elevator Tour in the Volcano" ),
		COINS
	},
	/* Shifting Sand Land */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "In the Talons of the Big Bird" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Shining Atop the Pyramid" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Inside the Ancient Pyramid" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Stand Tall on the Four Pillars" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Free Flying for 8 Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Pyramid Puzzle" ),
		COINS
	},
	/* Dire Dire Docks */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Board Bowser's Sub" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Chests in the Current" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Pole-Jumping for Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Through the Jet Stream" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "The Manta Ray's Reward" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Collect the Caps..." ),
		COINS
	},
	/* Snowman's Land */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Snowman's Big Head" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Chill with the Bully" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "In the Deep Freeze" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Whirl from the Freezing Pond" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Shell Shreddin' for Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Into the Igloo" ),
		COINS
	},
	/* Wet Dry World */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Shocking Arrow Lifts!" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Top o' the Town" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Secrets in the Shallows & Sky" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Express Evelator--Hurry Up!" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Go to Town for Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Quick Race through Downtown!" ),
		COINS
	},
	/* Tall, Tall Mountain */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Scale the Mountain" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Mystery of the Monkey's Cage" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Scary 'Shrooms, Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Mysterious Mountainside" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Breathtaking View from Bridge" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Blast to the Lonely Mushroom" ),
		COINS
	},
	/* Tiny-Huge Island */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Pluck the Piranha Flower" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "The Tip Top of the Huge Island" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Rematch with Koopa the Quick" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Five Itty Bitty Secrets" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Wiggler's Red Coins" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Make Wiggler Squirm" ),
		COINS
	},
	/* Tick Tock Clock */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Roll into the Cage" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "The Pit and the Pendulums" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Get a Hand" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Stomp on the Thwomp" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Timed Jumps on Moving Bars" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Stop Time for Red Coins" ),
		COINS
	},
	/* Rainbow Ride */
	{
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Cruiser Crossing the Rainbow" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "The Big House in the Sky" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Coins Amassed in a Maze" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Swingin' in the Breeze" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Tricky Triangles!" ),
		QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Somewhere Over the Rainbow" ),
		COINS
	},
	/* Bowser in the Dark World */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* Bowser in the Fire Sea */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* Bowser in the Sky */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* Peach's Secret Slide */
	{ "Box Star", "Timer Star", nullptr, nullptr, nullptr, nullptr },
	/* Cavern of the Metal Cap */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* Tower of the Wing Cap */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* Vanish Cap Under the Moat */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* Winged Mario Over the Rainbow */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* Secret Aquarium */
	{ REDS, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
	/* End Screen */
	{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }
};

const bool s_vanillaCannons[26] = {
	false,
	true, // Bob-Omb's Battlefield
	true, // Whomp's Fortress
	true, // Jolly Rodger Bay
	true, // Cool Cool Mountain
	false,
	false,
	false,
	true, // Shifting Sand Land
	false,
	true, // Snowman's Land
	true, // Wet Dry World
	true, // Tall Tall Mountain
	true, // Tiny-Huge Island
	false,
	true, // Rainbow Ride
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	true, // Winged Mario Over the Rainbow
	false,
	false
};

static const char *s_columnHeaders[9] = {
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Act 1 Star" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Act 2 Star" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Act 3 Star" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Act 4 Star" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Act 5 Star" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Act 6 Star" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "100 Coin Star" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Cannon Status" ),
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Coin High Score" )
};

static const char *s_courseNames[26] = {
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
	"End Screen",
	QT_TRANSLATE_NOOP( "SaveSlotEditorDialog", "Peach's Castle" )
};

static const SM64::LevelId s_levelIdMap[25] = {
	SM64::LevelId::BobOmbBattlefield,
	SM64::LevelId::WhompsFortress,
	SM64::LevelId::JollyRodgerBay,
	SM64::LevelId::CoolCoolMountain,
	SM64::LevelId::BigBoosHaunt,
	SM64::LevelId::HazyMazeCave,
	SM64::LevelId::LethalLavaLand,
	SM64::LevelId::ShiftingSandLand,
	SM64::LevelId::DireDireDocks,
	SM64::LevelId::SnowmansLands,
	SM64::LevelId::WetDryWorld,
	SM64::LevelId::TallTallMountain,
	SM64::LevelId::TinyHugeIsland,
	SM64::LevelId::TickTockClock,
	SM64::LevelId::RainbowRide,
	SM64::LevelId::BowserInTheDarkWorld,
	SM64::LevelId::BowserInTheFireSea,
	SM64::LevelId::BowserInTheSky,
	SM64::LevelId::PeachsSecretSlide,
	SM64::LevelId::CavernOfTheMetalCap,
	SM64::LevelId::TowerOfTheWingCap,
	SM64::LevelId::VanishCapUnderTheMoat,
	SM64::LevelId::WingedMarioOverTheRainbow,
	SM64::LevelId::SecretAquarium,
	SM64::LevelId::EndScreen
};

inline static void addLevelEntry( QComboBox *select, const char *name, SM64::LevelId levelId ) {
	select->addItem( QCoreApplication::translate( "SaveSlotEditorDialog", name ), QVariant::fromValue<uint>( (uint)levelId ) );
}

SaveSlotEditorDialog::SaveSlotEditorDialog( const fs::path &saveFilePath, int slot ) :
	QDialog( nullptr ),
	m_ui( new Ui::SaveSlotEditorDialog ),
	m_filePath( saveFilePath ),
	m_slot( slot )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
	UiUtil::scaleWindow( this );
	UiUtil::fixDialogButtonsOnWindows( this );

	m_starIcons.reserve( 26 * 8 );
	m_unusedStars.reserve( 77 );

	InputFile file( saveFilePath, true );
	const SM64::SaveFile saveFile = SM64::SaveFile::read( file );
	const SM64::SaveSlot &saveSlot = saveFile.slot( slot );

	addLevelEntry( m_ui->capLevel, QT_TR_NOOP( "-- None --" ), SM64::LevelId::Null );
	addLevelEntry( m_ui->capLevel, "Castle Grounds", SM64::LevelId::CastleGrounds );
	addLevelEntry( m_ui->capLevel, "Inside the Castle", SM64::LevelId::CastleInterior );
	addLevelEntry( m_ui->capLevel, "Castle Courtyard", SM64::LevelId::CastleCourtyard );
	for( int i = 0; i < 25; i++ ) {
		addLevelEntry( m_ui->capLevel, s_courseNames[i], s_levelIdMap[i] );
	}
	addLevelEntry( m_ui->capLevel, "Bowser 1", SM64::LevelId::Bowser1 );
	addLevelEntry( m_ui->capLevel, "Bowser 2", SM64::LevelId::Bowser2 );
	addLevelEntry( m_ui->capLevel, "Bowser 3", SM64::LevelId::Bowser3 );

	for( int i = 0; i < m_ui->capLevel->count(); i++ ) {
		if( m_ui->capLevel->itemData( i, Qt::UserRole ).value<uint>() == (uint)saveSlot.capLevel ) {
			m_ui->capLevel->setCurrentIndex( i );
			break;
		}
	}
	m_ui->capArea->setValue( (int)saveSlot.capArea );

	for( QAbstractButton *radio : m_ui->flagButtons->buttons() ) {
		const SM64::SaveFileFlag flag = (SM64::SaveFileFlag)radio->property( "saveFlag" ).value<uint>();
		radio->setChecked( Flags::has( saveSlot.flags, flag ) );
	}

	for( int i = 0; i < 9; i++ ) {
		m_ui->gridLayout->addWidget( new QLabel( tr( s_columnHeaders[i] ) ), 0, i+1, Qt::AlignCenter );
	}

	for( int i = 0; i < 26; i++ ) {
		m_ui->gridLayout->addWidget( new QLabel( tr( s_courseNames[i] ) ), i+1, 0, Qt::AlignRight | Qt::AlignVCenter );
	}

	QPixmap coinIcon = QIcon( ":/sm64/coin.svg" ).pixmap( 32, 32 );
	for( ubyte i = 1; i < 26; i++ ) {
		for( ubyte j = 0; j < 8; j++ ) {
			if( i == 25 && j == 7 ) continue;

			StarIcon *icon = new StarIcon( m_ui->starGrid, i, j, saveSlot.starFlags[i-1] & (1 << j) );
			m_starIcons.push_back( icon );
			if( j == 7 ) {
				if( !s_vanillaCannons[i] ) {
					m_unusedStars.push_back( icon );
				}
			} else {
				const char *starName = s_starNames[i][j];
				if( starName == nullptr ) {
					m_unusedStars.push_back( icon );
				} else {
					icon->setToolTip( tr( starName ) );
				}
			}

			m_ui->gridLayout->addWidget( icon, i, j+1 );
		}

		if( i <= 15 ) {
			QHBoxLayout *layout = new QHBoxLayout();
			QSpinBox *spinner = new QSpinBox();
			spinner->setMinimum( 0 );
			spinner->setMaximum( 255 );
			spinner->setValue( (int)saveSlot.coinScores[i-1] );
			QLabel *icon = new QLabel();
			icon->setPixmap( coinIcon );
			layout->addWidget( spinner, 1 );
			layout->addWidget( icon, 0 );
			m_coinInputs[i-1] = spinner;
			m_ui->gridLayout->addLayout( layout, i, 9 );
		}
	}

	for( ubyte j = 0; j < 7; j++ ) {
		StarIcon *icon = new StarIcon( m_ui->starGrid, 0, j, saveSlot.castleStars & (1 << j) );
		m_starIcons.push_back( icon );
		if( j < 5 ) {
			icon->setToolTip( tr( s_starNames[0][j] ) );
		} else {
			m_unusedStars.push_back( icon );
		}

		m_ui->gridLayout->addWidget( icon, 26, j+1 );
	}

}

SaveSlotEditorDialog::~SaveSlotEditorDialog() {
	UiUtil::clearLayout( m_ui->starGrid->layout() );
	delete m_ui;
}

void SaveSlotEditorDialog::setShowUnused( bool showUnused ) {
	m_ui->unusedFlagsGroup->setVisible( showUnused );
	for( StarIcon *icon : m_unusedStars ) {
		icon->setVisible( showUnused );
	}
}

void SaveSlotEditorDialog::accept() {
	using namespace SM64;

	SM64::SaveFile saveFile;
	{
		InputFile inFile( m_filePath, true );
		saveFile = SaveFile::read( inFile );
	}

	SaveSlot &slot = saveFile.slot( m_slot );

	slot.capLevel = (LevelId)m_ui->capLevel->currentData( Qt::UserRole ).value<uint>();
	slot.capArea = (ubyte)m_ui->capArea->value();

	slot.flags = SaveFileFlag::Exists;
	for( const QAbstractButton *radio : m_ui->flagButtons->buttons() ) {
		if( radio->isChecked() ) {
			slot.flags |= (SaveFileFlag)radio->property( "saveFlag" ).value<uint>();
		}
	}

	std::memset( slot.starFlags, 0, sizeof( slot.starFlags ) );
	slot.castleStars = 0;

	for( const StarIcon *icon : m_starIcons ) {
		if( !icon->isCollected() ) continue;

		if( icon->courseId() == 0 ) {
			slot.castleStars |= 1 << icon->starId();
		} else {
			slot.starFlags[icon->courseId() - 1] |= 1 << icon->starId();
		}
	}

	for( int i = 0; i < 15; i++ ) {
		slot.coinScores[i] = (ubyte)m_coinInputs[i]->value();
	}

	OutputFile outFile( m_filePath, true );
	saveFile.write( outFile );

	QDialog::accept();
}

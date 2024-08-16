#include "src/rhdc/ui/rhdc-hack-view.hpp"
#include <ui_rhdc-hack-view.h>

#include <QInputDialog>
#include <QMessageBox>
#include <QPalette>
#include <QAction>
#include <QMenu>
#include <QFile>
#include "src/polyfill/file-explorer.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/special-groups.hpp"
#include "src/core/retroarch.hpp"
#include "src/db/data-provider.hpp"
#include "src/ui/import-save-dialog.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/rhdc/ui/rhdc-layout-tester-dialog.hpp"
#include "src/rhdc/ui/rhdc-rating-dialog.hpp"
#include "src/rhdc/ui/rhdc-save-editor.hpp"
#include "src/rhdc/core/layout.hpp"

RhdcHackView::RhdcHackView( QWidget *parent ) :
	QWidget( parent ),
	m_ui( new Ui::RhdcHackView ),
	m_blockPlaylistEvents( false ),
	m_blockVersionEvents( false )
{
	m_ui->setupUi( this );
	m_ui->actionButton->setIcon( Icon::menu() );
	UiUtil::fixFontSizeOnMac( m_ui->hackTitleLabel );

	const RhdcUiState &uiState = FileController::loadUiState().rhdcList;
	m_sortAsc = uiState.sortAsc;

	m_ui->sortSelect->setCurrentIndex( (int)uiState.sortBy );
	m_ui->sortOrderButton->setIcon(
		uiState.sortBy == RhdcSorting::RandomShuffle ? Icon::shuffle() :
		(uiState.sortAsc ? Icon::sortAsc() : Icon::sortDsc())
	);

	UiUtil::setIconSize( m_ui->sortOrderButton, 16 );
	UiUtil::setIconSize( m_ui->actionButton, 16 );

	m_ui->searchBar->addAction( Icon::search(), QLineEdit::LeadingPosition );

	updatePlaylists();
	if( !uiState.playlist.empty() ) {
		m_blockPlaylistEvents = true;
		for( int i = 1; i < m_ui->playlistSelect->count(); i++ ) {
			if( m_ui->playlistSelect->itemData( i, Qt::UserRole ).toString().toStdString() == uiState.playlist ) {
				m_ui->playlistSelect->setCurrentIndex( i );
				break;
			}
		}
		m_blockPlaylistEvents = false;
	}

	m_ui->hackList->setPlaylist( uiState.playlist );
	m_ui->hackList->reloadAllHacks( false );
	sortingChanged();
	m_ui->hackList->selectHack( uiState.hackId );
	hackSelected( m_ui->hackList->currentHack() );

	QPalette descriptionPalette = m_ui->hackDescription->palette();
	descriptionPalette.setColor( QPalette::Active, QPalette::Base, descriptionPalette.color( QPalette::Active, QPalette::Window ) );
	descriptionPalette.setColor( QPalette::Inactive, QPalette::Base, descriptionPalette.color( QPalette::Inactive, QPalette::Window ) );
	descriptionPalette.setColor( QPalette::Disabled, QPalette::Base, descriptionPalette.color( QPalette::Disabled, QPalette::Window ) );
	m_ui->hackDescription->setPalette( descriptionPalette );
}

RhdcHackView::~RhdcHackView() {
	delete m_ui;
}

void RhdcHackView::showEvent( QShowEvent *event ) {
	reloadInternal( false );
	QWidget::showEvent( event );
}

void RhdcHackView::updatePlaylists() {
	const string previousPlaylist = m_ui->playlistSelect->currentData( Qt::UserRole ).toString().toStdString();

	m_blockPlaylistEvents = true;
	m_ui->playlistSelect->clear();

	int newIndex = previousPlaylist.empty() ? 0 : -1;
	m_ui->playlistSelect->addItem( tr( "All Hack Lists" ), QVariant::fromValue<QString>( "" ) );
	const std::vector<string> playlists = DataProvider::fetchRhdcPlaylists();

	int i = 1;
	for( const string &playlist : playlists ) {
		if( playlist == previousPlaylist ) newIndex = i;
		m_ui->playlistSelect->addItem( GroupName::localize( playlist ), QVariant::fromValue<QString>( playlist.c_str() ) );
		i++;
	}

	if( newIndex < 0 ) {
		m_ui->playlistSelect->setCurrentIndex( 0 );
		m_blockPlaylistEvents = false;
		playlistChanged();
	} else {
		m_ui->playlistSelect->setCurrentIndex( newIndex );
		m_blockPlaylistEvents = false;
	}

}

RhdcUiState RhdcHackView::uiState() const {
	const RhdcHackExt *hack = m_ui->hackList->currentHack();
	return RhdcUiState {
		(RhdcSorting)m_ui->sortSelect->currentIndex(),
		m_sortAsc,
		m_ui->playlistSelect->currentData( Qt::UserRole ).toString().toStdString(),
		hack ? hack->info.hackId : ""
	};
}

string RhdcHackView::getSelectedVersion() const {
	const RhdcHackExt *hack = m_ui->hackList->currentHack();
	if( hack == nullptr ) return "";
	return m_ui->versionSelect->currentData( Qt::UserRole ).toString().toStdString();
}

string RhdcHackView::getCurrentPlaylist() const {
	return m_ui->playlistSelect->currentData( Qt::UserRole ).toString().toStdString();
}

void RhdcHackView::hackSelected( const RhdcHackExt *hack ) {
	m_blockVersionEvents = true;
	m_ui->versionSelect->clear();
	m_ui->versionSelect->setEnabled( false );

	m_ui->hackTitleLabel->setEnabled( hack != nullptr );
	if( hack == nullptr ) {
		m_ui->hackTitleLabel->setText( "--" );
		m_ui->hackDescription->clear();
		m_ui->hackInfoPane->setDisabled( true );
		m_ui->authorsLabel->setText( tr( "Authors:" ) );
		m_ui->authorsList->setText( tr( "Unknown" ) );
		return;
	}

	m_ui->hackInfoPane->setDisabled( false );
	m_ui->authorsLabel->setText( (hack->authors.size() == 1) ? tr( "Author:" ) : tr( "Authors:" ) );
	m_ui->hackTitleLabel->setText( hack->info.name.c_str() );
	m_ui->hackDescription->setPlainText( hack->info.description.c_str() );

	if( hack->authors.empty() ) {
		/*: Shown when no authors are listed for a hack */
		m_ui->authorsList->setText( tr( "Unknown" ) );
	} else {
		QStringList authors;
		authors.reserve( (int)hack->authors.size() );
		for( const string &author : hack->authors ) {
			authors << author.c_str();
		}
		m_ui->authorsList->setText( authors.join( ", " ) );
	}

	int versionIndex = 0;
	bool foundVersion = false;
	const string activeVersion = DataProvider::getLastPlayedVersion( hack->info.hackId );
	const std::vector<RhdcHackVersion> versions = DataProvider::fetchRhdcHackVersions( hack->info.hackId );
	for( const RhdcHackVersion &version : versions ) {
		if( version.archived && !DataProvider::romWithSha1Exists( version.sha1 ) ) continue;
		
		if( activeVersion == version.sha1 ) {
			versionIndex = m_ui->versionSelect->count();
		}

		m_ui->versionSelect->addItem(
			version.name.c_str(),
			QVariant::fromValue<QString>( version.sha1.c_str() )
		);

		foundVersion = true;
	}

	if( foundVersion ) {
		m_ui->versionSelect->setEnabled( true );
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
		m_ui->versionSelect->setPlaceholderText( "---" );
	} else {
		m_ui->versionSelect->setPlaceholderText( "No Versions Available" );
	}
#else
	} else {
		m_ui->versionSelect->setCurrentText( "No Versions Available" );
	}
#endif

	m_ui->versionSelect->setCurrentIndex( versionIndex );
	m_blockVersionEvents = false;
	versionSelected();
}

bool RhdcHackView::tryGetHackIdAndDownloadUrl( string &hackId, string &downloadUrl ) const {
	const RhdcHackExt *hack = m_ui->hackList->currentHack();
	if( hack == nullptr ) return false;

	hackId = hack->info.hackId;
	downloadUrl = RhdcApi::getDownloadUrl( hack->info.slug, m_ui->versionSelect->currentText().toStdString() );
	return true;
}

string RhdcHackView::getSelectedHackId() const {
	const RhdcHackExt *hack = m_ui->hackList->currentHack();
	return (hack != nullptr) ? hack->info.hackId : ""s;
}

void RhdcHackView::changeSortOrder() {
	if( m_ui->sortSelect->currentIndex() != (int)RhdcSorting::RandomShuffle ) {
		m_sortAsc = !m_sortAsc;
		m_ui->sortOrderButton->setIcon( m_sortAsc ? Icon::sortAsc() : Icon::sortDsc() );
	}
	sortingChanged();
}

void RhdcHackView::versionSelected() {
	if( m_blockVersionEvents ) return;

	const RhdcHackExt *hack = m_ui->hackList->currentHack();
	if( hack == nullptr ) return;

	const string version = m_ui->versionSelect->currentData( Qt::UserRole ).toString().toStdString();
	DataProvider::setLastPlayedVersion( hack->info.hackId, version );

	emit romSelected();
}

void RhdcHackView::sortingChanged() {
	const RhdcSorting sortBy = (RhdcSorting)m_ui->sortSelect->currentIndex();
	if( sortBy == RhdcSorting::RandomShuffle ) {
		m_ui->sortOrderButton->setIcon( Icon::shuffle() );
	} else {
		m_ui->sortOrderButton->setIcon( m_sortAsc ? Icon::sortAsc() : Icon::sortDsc() );
	}

	m_ui->hackList->setSortOrder( sortBy, m_sortAsc );
}

void RhdcHackView::playlistChanged() {
	if( m_blockPlaylistEvents ) return;
	m_ui->hackList->setPlaylist( m_ui->playlistSelect->currentData( Qt::UserRole ).toString().toStdString() );
}

void RhdcHackView::reloadInternal( bool reshuffle ) {
	updatePlaylists();
	m_ui->hackList->reloadAllHacks( reshuffle );
	hackSelected( m_ui->hackList->currentHack() );
}

void RhdcHackView::reload() {
	reloadInternal( true );
}

enum class Hacktion : char {
	RateHack = 1,
	MarkComplete = 2,
	MarkIncomplete = 3,
	MoveToList = 4,
	CopyToList = 5,
	RemoveFromList = 6,
	DeleteSave = 7,
	EditSave = 8,
	Unfollow = 9,
	Uninstall = 10,
	ImportSave = 11,
	TestLayout = 12,
	ShowSave = 13
};

static inline void addHacktion(
	QMenu &menu,
	const QString &name,
	Hacktion actionType,
	const char *arg = nullptr
) {
	QAction *action = new QAction( name, &menu );
	action->setData( QVariant::fromValue<QString>( QString( QChar( (char)actionType ) ).append( arg ? arg : "" ) ) );
	menu.addAction( action );
}

static inline bool isAutoManagedPlaylist( const string &playlist ) {
	return(
		playlist == SpecialGroups::WantToPlay ||
		playlist == SpecialGroups::InProgress ||
		playlist == SpecialGroups::Completed
	);
}

// Get the playlists that the hack can be moved or copied to
static inline std::vector<string> getFreePlaylists( const string &hackId, bool &canRemoveFromPlaylist ) {
	const std::set<string> currentPlaylists = DataProvider::fetchGroupsForRhdcHack( hackId );
	const std::vector<string> allPlaylists = DataProvider::fetchRhdcPlaylists();

	std::vector<string> freePlaylists;
	for( const string &playlist : allPlaylists ) {
		if( !isAutoManagedPlaylist( playlist ) && currentPlaylists.count( playlist ) <= 0 ) {
			freePlaylists.push_back( playlist );
		}
	}

	canRemoveFromPlaylist &= (currentPlaylists.size() > 1);

	std::sort( freePlaylists.begin(), freePlaylists.end() );
	return freePlaylists;
}

static string createNewPlaylist() {
	const string playlist = QInputDialog::getText( nullptr, RhdcHackView::tr( "Create List" ), RhdcHackView::tr( "Enter a name for your new hack list" ) ).toStdString();
	if( playlist.empty() ) return playlist;

	for( const string &existingPlaylist : DataProvider::fetchAllGroups() ) {
		if( playlist == existingPlaylist || GroupName::getRhdcName( playlist ) == existingPlaylist ) {
			QMessageBox::critical( nullptr, RhdcHackView::tr( "List Exists" ), RhdcHackView::tr( "A hack list with this name already exists" ) );
			return "";
		}
	}

	DataProvider::addGroup( playlist );
	return playlist;
}

void RhdcHackView::actionButtonClicked() {
	const RhdcHackExt *hack = m_ui->hackList->currentHack();
	if( hack == nullptr ) return;

	const string currentPlaylist = m_ui->playlistSelect->currentData( Qt::UserRole ).toString().toStdString();

	RomFile romFile;
	const bool isDownloaded = DataProvider::tryFetchRomFileWithSha1(
		m_ui->versionSelect->currentData( Qt::UserRole ).toString().toStdString(),
		currentPlaylist,
		&romFile
	) && romFile.local;

	const fs::path saveFilePath = isDownloaded ? RetroArch::getSaveFilePath( romFile.path ) : fs::path();

	QMenu menu, moveMenu, copyMenu;

	addHacktion( menu, tr( "Rate Hack" ), Hacktion::RateHack );
	if( hack->info.starCount == 0 ) {
		if( hack->progress.completed ) {
			addHacktion( menu, tr( "Mark as Not Completed" ), Hacktion::MarkIncomplete );
		} else {
			addHacktion( menu, tr( "Mark as Completed" ), Hacktion::MarkComplete );
		}
	}

	menu.addSeparator();

	bool canRemoveFromPlaylist = !currentPlaylist.empty() && !isAutoManagedPlaylist( currentPlaylist );
	const std::vector<string> freePlaylists = getFreePlaylists( hack->info.hackId, canRemoveFromPlaylist );
	const bool canMove = !currentPlaylist.empty() && !isAutoManagedPlaylist( currentPlaylist );

	moveMenu.setTitle( tr( "Move to Hack List..." ) );
	copyMenu.setTitle( tr( "Copy to Hack List..." ) );

	for( const string &playlist : freePlaylists ) {
		addHacktion( moveMenu, GroupName::localize( playlist ), Hacktion::MoveToList, playlist.c_str() );
		addHacktion( copyMenu, GroupName::localize( playlist ), Hacktion::CopyToList, playlist.c_str() );
	}

	addHacktion( moveMenu, tr( "New List" ), Hacktion::MoveToList );
	addHacktion( copyMenu, tr( "New List" ), Hacktion::CopyToList );

	if( canMove ) menu.addMenu( &moveMenu );
	menu.addMenu( &copyMenu );

	if( canRemoveFromPlaylist ) {
		addHacktion( menu, tr( "Remove from '%1'" ).arg( GroupName::localize( currentPlaylist ) ), Hacktion::RemoveFromList );
	}

	menu.addSeparator();

	if( !saveFilePath.empty() ) {
		if( fs::existsSafe( saveFilePath ) ) {
			addHacktion( menu, tr( "Delete Save File" ), Hacktion::DeleteSave );
		}

		addHacktion( menu, tr( "Edit Save File" ), Hacktion::EditSave );
		addHacktion( menu, tr( "Import Project64 Save File" ), Hacktion::ImportSave );
		addHacktion( menu, tr( "Show Save File" ), Hacktion::ShowSave );
		menu.addSeparator();
	}

	addHacktion( menu, tr( "Unfollow Hack" ), Hacktion::Unfollow );
	if( isDownloaded ) {
		addHacktion( menu, tr( "Uninstall Hack Version" ), Hacktion::Uninstall );
		menu.addSeparator();
		addHacktion( menu, "[RHDC] Test Star Layout", Hacktion::TestLayout );
	}

	const QAction *selectedAction = menu.exec( m_ui->actionButton->mapToGlobal( m_ui->actionButton->rect().bottomLeft() ) );
	if( selectedAction == nullptr ) return;

	const QString actionString = selectedAction->data().toString();
	if( actionString.isNull() || actionString.isEmpty() ) return;

	switch( (Hacktion)actionString.at( 0 ).toLatin1() ) {
		case Hacktion::RateHack: {
			RhdcRatingDialog dialog;
			dialog.setRatings( hack->progress.rating, hack->progress.difficulty, hack->info.category == "Kaizo" );
			if( dialog.exec() == QDialog::Accepted ) {
				DataProvider::updateRhdcHackRating( hack->info.hackId, dialog.getQuality(), dialog.getDifficulty() );
				RhdcApi::submitRatingsAsync(
					hack->info.hackId, dialog.getQuality(), dialog.getDifficulty(),
					[](){},
					RhdcApi::logApiError( "Failed to submit hack ratings" )
				);
				m_ui->hackList->reloadAllHacks( false );
			}
			break;
		}
		case Hacktion::MarkComplete:
			for( const RhdcHackVersion &version : DataProvider::fetchRhdcHackVersions( hack->info.hackId ) ) {
				DataProvider::removeFromGroup( version.sha1, SpecialGroups::WantToPlay );
				DataProvider::removeFromGroup( version.sha1, SpecialGroups::InProgress );
				DataProvider::addToGroup( version.sha1, SpecialGroups::Completed );
			}
			DataProvider::updateRhdcStarProgress( hack->info.hackId, 0, true );
			if( hack->info.submitStarpower ) {
				RhdcApi::submitCompletionAsync( hack->info.hackId, true, [](){}, RhdcApi::logApiError( "Failed to submit hack progress" ) );
			}

			m_ui->hackList->reloadAllHacks( false );
			break;
		case Hacktion::MarkIncomplete:
			for( const RhdcHackVersion &version : DataProvider::fetchRhdcHackVersions( hack->info.hackId ) ) {
				DataProvider::removeFromGroup( version.sha1, SpecialGroups::Completed );
				if( hack->playTime >=  5 * 60 * 1000 ) {
					DataProvider::removeFromGroup( version.sha1, SpecialGroups::WantToPlay );
					DataProvider::addToGroup( version.sha1, SpecialGroups::InProgress );
				} else {
					DataProvider::removeFromGroup( version.sha1, SpecialGroups::InProgress );
					DataProvider::addToGroup( version.sha1, SpecialGroups::WantToPlay );
				}
			}
			DataProvider::updateRhdcStarProgress( hack->info.hackId, 0, false );
			if( hack->info.submitStarpower ) {
				RhdcApi::submitCompletionAsync( hack->info.hackId, false, [](){}, RhdcApi::logApiError( "Failed to submit hack progress" ) );
			}

			m_ui->hackList->reloadAllHacks( false );
			break;
		case Hacktion::MoveToList: {
			string playlist = actionString.toStdString().substr( 1 );
			if( playlist.empty() ) {
				playlist = createNewPlaylist();
				if( playlist.empty() ) return;
			}

			for( const RhdcHackVersion &version : DataProvider::fetchRhdcHackVersions( hack->info.hackId ) ) {
				DataProvider::addToGroup( version.sha1, playlist );
				DataProvider::removeFromGroup( version.sha1, currentPlaylist );
			}
			RhdcApi::addHackToListAsync( hack->info.hackId, GroupName::getRhdcName( playlist ), [](){}, RhdcApi::logApiError( "Failed to add hack to list" ) );
			RhdcApi::removeHackFromListAsync( hack->info.hackId, GroupName::getRhdcName( currentPlaylist ), [](){}, RhdcApi::logApiError( "Failed to remove hack from list" ) );
			reloadInternal( false );
			break;
		}
		case Hacktion::CopyToList: {
			string playlist = actionString.toStdString().substr( 1 );
			if( playlist.empty() ) {
				playlist = createNewPlaylist();
				if( playlist.empty() ) return;
			}

			for( const RhdcHackVersion &version : DataProvider::fetchRhdcHackVersions( hack->info.hackId ) ) {
				DataProvider::addToGroup( version.sha1, playlist );
			}
			RhdcApi::addHackToListAsync( hack->info.hackId, GroupName::getRhdcName( playlist ), [](){}, RhdcApi::logApiError( "Failed to add hack to list" ) );
			reloadInternal( false );
			break;
		}
		case Hacktion::RemoveFromList:
			for( const RhdcHackVersion &version : DataProvider::fetchRhdcHackVersions( hack->info.hackId ) ) {
				DataProvider::removeFromGroup( version.sha1, currentPlaylist );
			}
			RhdcApi::removeHackFromListAsync( hack->info.hackId, GroupName::getRhdcName( currentPlaylist ), [](){}, RhdcApi::logApiError( "Failed to remove hack from list" ) );
			reloadInternal( false );
			break;
		case Hacktion::DeleteSave:
			if( QMessageBox::question( nullptr, tr( "Confirm Delete" ), tr( "Are you sure you want to delete your save file?" ) ) == QMessageBox::Yes ) {
				fs::error_code err;
				fs::remove( saveFilePath, err );
			}
			break;
		case Hacktion::EditSave:
			if( !fs::existsSafe( saveFilePath ) ) {
				QFile saveFile( saveFilePath.u8string().c_str() );
				saveFile.open( QFile::WriteOnly | QFile::Truncate );

				QByteArray zeroBuffer;
				zeroBuffer.fill( '\0', 296960 );

				saveFile.write( zeroBuffer );
				saveFile.flush();
			}

			if( StarLayout::hasLayout( hack->info.hackId ) ) {
				RhdcSaveEditor::exec( saveFilePath, hack->info.hackId );
			} else {
				emit editSave( saveFilePath );
			}
			break;
		case Hacktion::ShowSave:
			FileExplorer::showFile( saveFilePath );
			break;
		case Hacktion::Unfollow:
			if( QMessageBox::question( nullptr, tr( "Confirm Unfollow" ), tr( "Are you sure you want to unfollow this hack? The ROM will not be deleted from your computer, but it will be removed from all of your hack lists on romhacking.com and your progress will no longer be synced with romhacking.com." ) ) == QMessageBox::Yes ) {
				for( const string &playlist : DataProvider::fetchGroupsForRhdcHack( hack->info.hackId ) ) {
					RhdcApi::removeHackFromListAsync( hack->info.hackId, GroupName::getRhdcName( playlist ), [](){}, RhdcApi::logApiError( "Failed to remove hack from playlist" ) );
				}
				DataProvider::forgetHack( hack->info.hackId );
				m_ui->hackList->reloadAllHacks( false );
			}
			break;
		case Hacktion::Uninstall:
			if( QMessageBox::question( nullptr, tr( "Confirm Delete" ), tr( "Are you sure you want to delete this ROM file from your computer?" ) ) == QMessageBox::Yes ) {
				DataProvider::deleteRomPath( romFile.path );
				fs::error_code err;
				fs::remove( romFile.path, err );
				m_ui->hackList->reloadHack( hack->info.hackId );
			}
			break;
		case Hacktion::ImportSave:
			ImportSaveDialog::exec( saveFilePath );
			break;
		case Hacktion::TestLayout: {
			window()->hide();
			RhdcLayoutTesterDialog *dialog = new RhdcLayoutTesterDialog( romFile.path );
			connect( dialog, &RhdcLayoutTesterDialog::finished, this, [this](int){
				this->window()->show();
			});
			dialog->show();
			break;
		}
	}
}

RomSettingsWidgetSlot *RhdcHackView::settingsSlot() const {
	return m_ui->settingsSlot;
}

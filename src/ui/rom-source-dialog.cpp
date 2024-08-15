#include "src/ui/rom-source-dialog.hpp"
#include "ui_rom-source-dialog.h"

#include <QMessageBox>
#include <QButtonGroup>
#include "src/core/file-controller.hpp"
#include "src/core/special-groups.hpp"
#include "src/core/bps.hpp"
#include "src/db/data-provider.hpp"
#include "src/db/transaction.hpp"
#include "src/ui/manage-groups-dialog.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/polyfill/file-dialog.hpp"
#include "src/polyfill/crash.hpp"

RomSourceDialog::RomSourceDialog( QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::RomSourceDialog ),
	m_currentSource( nullptr ),
	m_ignoreEvents( false )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
	UiUtil::scaleWindow( this );
	UiUtil::fixDialogButtonsOnWindows( this );

	m_ui->browseButton->setIcon( Icon::browse() );
	m_ui->addSourceButton->setIcon( Icon::add() );
	m_ui->deleteSourceButton->setIcon( Icon::delet() );
	m_ui->addRomButton->setIcon( Icon::add() );
	m_ui->forgetRomButton->setIcon( Icon::delet() );
	m_ui->patchFileBrowseButton->setIcon( Icon::browse() );
	m_ui->baseRomBrowseButton->setIcon( Icon::browse() );

#ifdef __APPLE__
	QFont applyButtonFont = m_ui->applyPatchButton->font();
	applyButtonFont.setPointSizeF( 20.8 );
	m_ui->applyPatchButton->setFont( applyButtonFont );
#endif

	connect(
		m_ui->baseRomRadioGroup,
		SIGNAL( buttonToggled( QAbstractButton*, bool ) ),
		this,
		SLOT( baseRomModeChanged() )
	);
}

RomSourceDialog::~RomSourceDialog() {
	delete m_ui;
}

static inline void setItalicised( QListWidgetItem *item, bool useItalics ) {
	QFont font = item->font();
	font.setItalic( useItalics );
	item->setFont( font );
}

void RomSourceDialog::showEvent( QShowEvent *event ) {
	m_ignoreEvents = true;
	m_sources = FileController::loadRomSources();
	m_ui->romSourceList->clear();
	m_ui->additionalRomsList->clear();

	for( const RomSource &source : m_sources ) {
		m_ui->romSourceList->addItem( source.folder.u8string().c_str() );
	}

	for( const fs::path &romPath : DataProvider::fetchManualRomPaths() ) {
		m_ui->additionalRomsList->addItem( romPath.u8string().c_str() );
	}
	m_ui->additionalRomsList->sortItems();

	if( m_sources.empty() ) {
		QListWidgetItem *temporarySource = new QListWidgetItem();
		temporarySource->setText( tr( "New Source" ).append( "..." ) );
		setItalicised( temporarySource, true );
		m_ui->romSourceList->addItem( temporarySource );
	}

	m_currentSource = nullptr;
	m_ui->romSourceList->setCurrentRow( 0 );

	refreshGroups();
	romSelected();
	m_ignoreEvents = false;

	m_ui->patchFilePath->clear();
	m_ui->baseRomPath->clear();
	m_ui->autoBaseRomRadio->setChecked( true );
	m_ui->applyPatchButton->setEnabled( false );

	sourceSelected();
	QDialog::showEvent( event );
}

void RomSourceDialog::closeEvent( QCloseEvent *event ) {
	accept();
	QDialog::closeEvent( event );
}

void RomSourceDialog::accept() {
	save();

	SqlTransaction transaction;
	DataProvider::deleteAllManualRomPaths();
	for( int i = 0; i < m_ui->additionalRomsList->count(); i++ ) {
		DataProvider::addManualRomPath(
			fs::to_path( m_ui->additionalRomsList->item( i )->text().toStdString() )
		);
	}
	transaction.commit();

	QDialog::accept();
}

void RomSourceDialog::save() {
	saveCurrentSource();
	FileController::saveRomSources( m_sources );
}

void RomSourceDialog::saveCurrentSource() {
	if( m_currentSource == nullptr ) return;
	pl_assert( !m_ui->sourcePath->text().isEmpty() );

	m_currentSource->folder = fs::to_path( m_ui->sourcePath->text().toStdString() );
	m_currentSource->recursive = m_ui->recursiveCheckbox->isChecked();
	m_currentSource->ignoreHidden = m_ui->ignoreHiddenCheckbox->isChecked();
	m_currentSource->maxDepth = (ubyte)m_ui->maxDepthSpinner->value();
	m_currentSource->followSymlinks = m_ui->followSymlinksCheckbox->isChecked();
	m_currentSource->autoTags.clear();
	for( const auto &i : m_groupMap ) {
		if( i.second->isChecked() ) {
			m_currentSource->autoTags.insert( i.first );
		}
	}
}

void RomSourceDialog::refreshGroups() {
	const std::set<string> groups = DataProvider::fetchPersistentGroups();
	UiUtil::clearLayout( m_ui->groupList->layout() );
	m_groupMap.clear();
	for( const string &group : groups ) {
		QCheckBox *checkbox = new QCheckBox( GroupName::localize( group ), m_ui->groupList );
		m_groupMap[group] = checkbox;
		m_ui->groupList->layout()->addWidget( checkbox );
	}
	static_cast<QVBoxLayout*>( m_ui->groupList->layout() )->addStretch( 1 );

	if( m_currentSource != nullptr ) {
		for( auto &i : m_groupMap ) {
			i.second->setChecked( m_currentSource->autoTags.count( i.first ) > 0 );
		}
	}
}

void RomSourceDialog::sourceSelected() {
	if( m_ignoreEvents ) return;

	saveCurrentSource();

	const bool isTempSource = (m_ui->romSourceList->currentRow() >= (int)m_sources.size());
	m_ui->addSourceButton->setEnabled( !isTempSource );
	m_ui->deleteSourceButton->setEnabled( !isTempSource );

	if( isTempSource ) {
		m_currentSource = nullptr;

		m_ui->sourcePath->clear();
		m_ui->recursiveCheckbox->setChecked( false );
		m_ui->recursiveSearchOptions->setEnabled( false );
		m_ui->ignoreHiddenCheckbox->setChecked( true );
		m_ui->maxDepthSpinner->setValue( 5 );
		m_ui->followSymlinksCheckbox->setChecked( true );
		for( auto &i : m_groupMap ) i.second->setChecked( false );
	} else {
		m_currentSource = &m_sources.at( m_ui->romSourceList->currentRow() );
		const RomSource &source = *m_currentSource;

		m_ui->sourcePath->setText( source.folder.u8string().c_str() );
		m_ui->recursiveCheckbox->setChecked( source.recursive );
		m_ui->recursiveSearchOptions->setEnabled( source.recursive );
		m_ui->ignoreHiddenCheckbox->setChecked( source.ignoreHidden );
		m_ui->maxDepthSpinner->setValue( source.maxDepth );
		m_ui->followSymlinksCheckbox->setChecked( source.followSymlinks );
		for( auto &i : m_groupMap ) {
			i.second->setChecked( source.autoTags.count( i.first ) > 0 );
		}

		if( m_ui->romSourceList->count() > (int)m_sources.size() ) {
			m_ignoreEvents = true;
			delete m_ui->romSourceList->takeItem( (int)m_sources.size() );
			m_ignoreEvents = false;
		}
	}
}

void RomSourceDialog::addSource() {
	pl_assert( m_sources.empty() || m_ui->romSourceList->currentRow() < (int)m_sources.size() );

	saveCurrentSource();
	m_currentSource = nullptr;

	m_ignoreEvents = true;
	QListWidgetItem *temporarySource = new QListWidgetItem();
	temporarySource->setText( "New Source..." );
	setItalicised( temporarySource, true );
	m_ui->romSourceList->addItem( temporarySource );
	m_ui->romSourceList->setCurrentItem( temporarySource );
	m_ignoreEvents = false;
	sourceSelected();
}

void RomSourceDialog::deleteSource() {
	pl_assert( m_ui->romSourceList->currentRow() < (int)m_sources.size() );
	pl_assert( m_ui->romSourceList->currentRow() >= 0 );

	m_currentSource = nullptr;
	if( m_sources.size() == 1 ) {
		m_sources.clear();
		m_ignoreEvents = true;
		m_ui->romSourceList->clear();
		m_ignoreEvents = false;
		addSource();
	} else {
		int previousIndex = m_ui->romSourceList->currentRow();
		m_sources.erase( m_sources.begin() + previousIndex );
		m_ignoreEvents = true;
		delete m_ui->romSourceList->takeItem( previousIndex );
		if( previousIndex == m_ui->romSourceList->count() ) previousIndex--;
		m_ui->romSourceList->setCurrentRow( previousIndex );
		m_ignoreEvents = false;
		sourceSelected();
	}

}

void RomSourceDialog::browse() {
	const fs::path filePath = FileDialog::getDirectory( "Select a folder" );
	if( filePath.empty() ) return;
	m_ui->sourcePath->setText( filePath.u8string().c_str() );

	if( m_currentSource == nullptr ) {
		pl_assert( m_ui->romSourceList->currentRow() == (int)m_sources.size() );
		m_ui->romSourceList->currentItem()->setText( filePath.u8string().c_str() );
		setItalicised( m_ui->romSourceList->currentItem(), false );
		m_sources.push_back( RomSource() );
		m_currentSource = &m_sources.back();
		m_ui->addSourceButton->setEnabled( true );
		m_ui->deleteSourceButton->setEnabled( false );
		saveCurrentSource();
	}
}

void RomSourceDialog::manageGroups() {
	ManageGroupsDialog *dialog = new ManageGroupsDialog();
	dialog->exec();
	delete dialog;
	refreshGroups();
}

void RomSourceDialog::romSelected() {
	m_ui->forgetRomButton->setEnabled( !m_ui->additionalRomsList->selectedItems().empty() );
}

void RomSourceDialog::addRomToList( const QString &path ) {
	if( m_ui->additionalRomsList->findItems( path, Qt::MatchExactly ).empty() ) {
		m_ui->additionalRomsList->addItem( path );
	}
}

void RomSourceDialog::addRom() {
	const std::vector<fs::path> roms = FileDialog::getFiles(
		tr( "Select one or more ROMs" ).toStdString().c_str(),
		{ { "N64 ROM", "*.z64;*.n64;*.v64" } }
	);

	if( roms.empty() ) return;

	for( const fs::path &romPath : roms ) {
		addRomToList( romPath.u8string().c_str() );
	}

	m_ui->additionalRomsList->sortItems();
	romSelected();
}

void RomSourceDialog::forgetRom() {
	pl_assert( !m_ui->additionalRomsList->selectedItems().empty() );
	delete m_ui->additionalRomsList->takeItem( m_ui->additionalRomsList->currentRow() );
	romSelected();
}

void RomSourceDialog::browseForPatch() {
	const fs::path path = FileDialog::getFile(
		tr( "Select a .bps patch file" ).toStdString().c_str(),
		{ { tr( "Patch File" ).toStdString().c_str(), "*.bps" } }
	);

	if( path.empty() ) return;
	m_ui->patchFilePath->setText( path.u8string().c_str() );
	refreshApplyPatchButton();
}

void RomSourceDialog::browseForBaseRom() {
	const fs::path path = FileDialog::getFile(
		tr( "Select a ROM" ).toStdString().c_str(),
		{ { "N64 ROM", "*.z64;*.n64;*.v64" } }
	);

	if( path.empty() ) return;
	m_ui->baseRomPath->setText( path.u8string().c_str() );
	refreshApplyPatchButton();
}

void RomSourceDialog::applyPatch() {
	Bps::BpsApplyError result;
	fs::path romPath;
	if( m_ui->autoBaseRomRadio->isChecked() ) {
		result = Bps::tryApplyBps(
			fs::to_path( m_ui->patchFilePath->text().toStdString() ),
			romPath
		);
	} else {
		result = Bps::tryApplyBps(
			fs::to_path( m_ui->patchFilePath->text().toStdString() ),
			fs::to_path( m_ui->baseRomPath->text().toStdString() ),
			romPath
		);
	}

	switch( result ) {
		case Bps::BpsApplyError::None: {
			const QString path = QString( romPath.u8string().c_str() );

			QMessageBox::information( this, tr( "Patch Applied" ), tr( "Saved patched rom to %1" ).arg( path ) );

			if( !RomUtil::coveredBySearchPath( fs::to_path( path.toStdString() ) ) ) {
				addRomToList( path );
				m_ui->additionalRomsList->sortItems();
				romSelected();
			}

			m_ui->patchFilePath->clear();
			m_ui->baseRomPath->clear();
			m_ui->autoBaseRomRadio->setChecked( true );
			m_ui->applyPatchButton->setEnabled( false );
			break;
		}
		case Bps::BpsApplyError::InvalidBps:
			QMessageBox::critical( this, tr( "Patch Failed" ), tr( "Failed to patch ROM. The .bps patch appears to be invalid." ) );
			break;
		case Bps::BpsApplyError::PatchFailed:
			QMessageBox::critical( this, tr( "Patch Failed" ), tr( "Failed to patch ROM. The base ROM does not match what the patch expects." ) );
			break;
		case Bps::BpsApplyError::NoBaseRom:
			if( m_ui->autoBaseRomRadio->isChecked() ) {
				QMessageBox::critical( this, tr( "Patch Failed" ), tr( "Failed to patch ROM. The base rom required to patch is not known to Parallel Launcher." ) );
			} else {
				QMessageBox::critical( this, tr( "Patch Failed" ), tr( "Failed to patch ROM. The base ROM is missing or invalid." ) );
			}
			break;
		case Bps::BpsApplyError::WriteError:
			QMessageBox::critical( this, tr( "Patch Failed" ), tr( "Failed to patch ROM. An error occurred while writing the patched ROM to disk." ) );
			break;
	}

}

void RomSourceDialog::baseRomModeChanged() {
	m_ui->baseRomGroup->setEnabled( m_ui->manualBaseRomRadio->isChecked() );
	refreshApplyPatchButton();
}

void RomSourceDialog::refreshApplyPatchButton() {
	m_ui->applyPatchButton->setEnabled(
		!m_ui->patchFilePath->text().isEmpty() && (
			m_ui->autoBaseRomRadio->isChecked() ||
			!m_ui->baseRomPath->text().isEmpty()
		)
	);
}

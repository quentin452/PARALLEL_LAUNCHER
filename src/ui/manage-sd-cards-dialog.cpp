#include "src/ui/manage-sd-cards-dialog.hpp"
#include "ui_manage-sd-cards-dialog.h"

#include <QDialogButtonBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>
#include <unordered_set>
#include <iostream>
#include <thread>
#include "src/polyfill/file-explorer.hpp"
#include "src/polyfill/file-dialog.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/logging.hpp"
#include "src/core/qthread.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/ui/import-sd-card-dialog.hpp"
#include "src/ui/device-busy-dialog.hpp"

#ifdef _WIN32
#include <QApplication>
#include <QStyle>
#include <algorithm>
#include <cctype>

static inline string uppercase( string str ) {
	for( size_t i = 0; i < str.length(); i++ ) {
		str[i] = (char)std::toupper( (int)str[i] );
	}
	return str;
}

static bool sdCardInfoCmp( const SdCardInfo &a, const SdCardInfo &b ) {
	return a.name < b.name;
}
#endif

ManageSdCardsDialog::ManageSdCardsDialog( QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::ManageSdCardsDialog )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
	UiUtil::scaleWindow( this );
	UiUtil::fixDialogButtonsOnWindows( this );

	m_alive = std::make_shared<bool>( true );

	m_ui->loadingBar->setVisible( false );
	m_creating = false;

	m_ui->createButton->setIcon( Icon::add() );
	m_ui->importButton->setIcon( Icon::browse() );
	m_ui->reformatButton->setIcon( Icon::recycle() );
	m_ui->deleteButton->setIcon( Icon::delet() );
	m_ui->saveButton->setIcon( Icon::save() );
	m_ui->cancelButton->setIcon( Icon::cancel() );
	m_ui->cloneButton->setIcon( Icon::clone() );

#ifdef _WIN32
	QIcon uacIcon = QApplication::style()->standardIcon( QStyle::SP_VistaShield );
	m_ui->browseFilesButton->setIcon( uacIcon.isNull() ? Icon::mount() : uacIcon );
#else
	m_ui->browseFilesButton->setIcon( Icon::mount() );
#endif

	refreshList();
}

ManageSdCardsDialog::~ManageSdCardsDialog() {
	*m_alive = false;
	delete m_ui;
}

string ManageSdCardsDialog::generateName() const {
	HashSet<string> usedNames;
	usedNames.reserve( m_cards.size() );

	for( const SdCardInfo &card : m_cards ) {
		#ifdef _WIN32
			usedNames.insert( uppercase( card.name ) );
		#else
			usedNames.insert( card.name );
		#endif
	}

	const string prefix = "SDCARD";
	for( int i = 0; true; i++ ) {
		const string name = prefix + Number::toString( i );
		if( usedNames.find( name ) == usedNames.end() ) return name;
	}
}

void ManageSdCardsDialog::refreshList() {
	m_cards = SdCardManager::fetchAll();
#ifdef _WIN32
	foreach_cvalue( mountedCard, m_mountedCards ) {
		m_cards.push_back( mountedCard->info() );
	}
	std::sort( m_cards.begin(), m_cards.end(), sdCardInfoCmp );
#endif

	m_ui->cardSelector->clear();
	for( const SdCardInfo &card : m_cards ) {
		m_ui->cardSelector->addItem( QString( card.name.c_str() ) );
	}

	if( !m_creating ) {
		if( m_cards.empty() ) {
			beginCreate();
		} else {
			m_ui->cardSelector->setCurrentRow( 0 );
			cardSelected( 0 );
		}
	}
}

void ManageSdCardsDialog::reformat() {
	if( QMessageBox::question(
		this,
		tr( "Confirm Reformat" ),
		tr( "Are you sure you want to reformat this virtual SD card? All data currently on it will be lost." )
	) != QMessageBox::StandardButton::Yes ) {
		return;
	}

	const string &cardName = m_ui->nameInput->text().toStdString();
	if( !unmountOne( cardName ) ) return;

	SdCardManager::deleteCard( cardName );
	createCard();
}

static inline QString makeSafeName( const QString &name ) {
	QString safeName;
	safeName.reserve( name.size() );
	for( const QChar &c : name ) {
		const char cc = c.toLatin1();
		if( (ubyte)cc < 32 || (ubyte)cc > 126 ) continue;
		if( cc == '<' || cc == '>' || cc == ':' || cc == '"' || cc == '/' || cc == '\\' || cc == '|' || cc == '?' || cc == '*' ) continue;
		safeName += c;
	}

	return safeName;
}

static void validateName( QString name, QLineEdit *input, QPushButton *saveButton, const std::vector<SdCardInfo> &cards ) {
	const QString safeName = makeSafeName( name );
	if( name != safeName ) {
		input->setText( safeName );
		name = safeName;
	}

	if( name.trimmed().isEmpty() ) {
		saveButton->setEnabled( false );
		return;
	}

#ifdef _WIN32
	const QString uname = name.toUpper();
	if(
		uname == QString( "CON" ) ||
		uname == QString( "PRN" ) ||
		uname == QString( "AUX" ) ||
		uname == QString( "NUL" ) ||
		(
			uname.length() == 4 &&
			(uname.startsWith( "COM" ) || uname.startsWith( "LPT" ) ) &&
			uname[3].isDigit()
		)
	) {
		saveButton->setEnabled( false );
		return;
	}
#endif

	const string nameStr = name.toStdString();
	for( const SdCardInfo &card : cards ) {
#ifdef _WIN32
		if( uppercase( nameStr ) == uppercase( card.name ) ) {
#else
		if( nameStr == card.name ) {
#endif
			saveButton->setEnabled( false );
			return;
		}
	}

	saveButton->setEnabled( true );
}

void ManageSdCardsDialog::nameChanged( QString name ) {
	if( !m_creating ) return;
	validateName( name, m_ui->nameInput, m_ui->saveButton, m_cards );
}

void ManageSdCardsDialog::formatChanged( int format ) {
	switch( (SdCardFormat)format ) {
		case SdCardFormat::FAT12:
			m_ui->sizeSpinner->setRange( 1, 32 );
			break;
		case SdCardFormat::FAT16:
			m_ui->sizeSpinner->setRange( 32, 2048 );
			break;
		case SdCardFormat::FAT32:
			m_ui->sizeSpinner->setRange( 32, 2097152 );
			break;
		default:
			m_ui->sizeSpinner->setRange( 1, 2097152 );
			break;
	}
}

void ManageSdCardsDialog::deleteCard() {
	if( QMessageBox::question(
		this,
		tr( "Confirm Deletion" ),
		tr( "Are you sure you want to delete this virtual SD card? This action cannot be undone." )
	) != QMessageBox::StandardButton::Yes ) {
		return;
	}

	const string &cardName = m_ui->nameInput->text().toStdString();
	if( !unmountOne( cardName ) ) return;

	SdCardManager::deleteCard( cardName );
	refreshList();
}

void ManageSdCardsDialog::createAsync(
	QString name,
	std::shared_ptr<AsyncProcess> task,
	std::shared_ptr<bool> alive,
	ManageSdCardsDialog *dialog
) {
	const int64 status = task->join();
	if( !*alive ) return;
	
	QtThread::safeAsync( [=](){
		if( !*alive ) return;

		dialog->m_ui->loadingBar->setVisible( false );
		dialog->m_ui->cardOptions->setEnabled( true );
		dialog->m_ui->editButtonTray->setEnabled( true );
		dialog->m_ui->createButtonTray->setEnabled( true );
		dialog->m_ui->dialogButtonTray->setEnabled( true );

		if( status != 0 ) {
			QMessageBox::critical( dialog, tr( "Unexpected Error" ), tr( "Failed to create the virtual SD card because of an unknown error." ) );
			SdCardManager::deleteCard( name.toStdString() );
			return;
		}

		dialog->m_ui->cardSelector->setEnabled( true );
		dialog->m_ui->createButton->setEnabled( true );
		dialog->m_ui->importButton->setEnabled( true );
		dialog->m_ui->editButtonTray->setVisible( true );
		dialog->m_ui->createButtonTray->setVisible( false );

		dialog->m_creating = false;
		dialog->refreshList();

		for( int i = 0; i < dialog->m_ui->cardSelector->count(); i++ ) {
			if( dialog->m_ui->cardSelector->item( i )->text() == name ) {
				dialog->m_ui->cardSelector->setCurrentRow( i );
				dialog->cardSelected( i );
				break;
			}
		}
	});
}

void ManageSdCardsDialog::createCard() {
	AsyncProcess proc;
	try {
		proc = SdCardManager::createCardAsync({
			m_ui->nameInput->text().toStdString(),
			(SdCardFormat)m_ui->filesystemSelect->currentIndex(),
			(uint)m_ui->sizeSpinner->value() * 1024
		});
	} catch( ... ) {
		logError( "Failed to launch process to format SD card." );
		QMessageBox::critical( this, tr( "Unexpected Error" ), tr( "Failed to create the virtual SD card because of an unknown error." ) );
	}

	m_ui->loadingBar->setVisible( true );
	m_ui->cardOptions->setEnabled( false );
	m_ui->editButtonTray->setEnabled( false );
	m_ui->createButtonTray->setEnabled( false );
	m_ui->dialogButtonTray->setEnabled( false );

	std::shared_ptr<AsyncProcess> task( new AsyncProcess );
	*task = std::move( proc );

	std::thread asyncTask( ManageSdCardsDialog::createAsync, m_ui->nameInput->text(), task, m_alive, this );
	asyncTask.detach();
}

void ManageSdCardsDialog::importCard() {
	const fs::path filePath = FileDialog::getFile(
		tr( "Import SD Card" ).toStdString().c_str(),
		{{ tr( "Raw Disk Image" ).toStdString().c_str(), "*.iso" }}
	);

	if( filePath.empty() ) return;

	fs::error_code err;
	const uintmax_t fileSize = fs::file_size( filePath, err );
	if( fileSize == 0 || err ) {
		QMessageBox::critical( this, tr( "Invalid disk image" ), tr( "The specified file is not a valid disk image." ) );
		return;
	} else if( fileSize > 2199023255552ull ) {
		QMessageBox::critical( this, tr( "Disk image too large" ), tr( "The disk image could not be imported because it is larger than 2 TiB." ) );
		return;
	}

	ImportSdCardDialog dialog( this, filePath );
	if( dialog.exec() == QDialog::Accepted ) {
		refreshList();
		discard();

		const QString cardName = filePath.stem().u8string().c_str();
		for( int i = 0; i < m_ui->cardSelector->count(); i++ ) {
			if( m_ui->cardSelector->item( i )->text() == cardName ) {
				m_ui->cardSelector->setCurrentRow( i );
				cardSelected( i );
				return;
			}
		}
	}
}

void ManageSdCardsDialog::discard() {
	m_creating = false;

	m_ui->cardSelector->setEnabled( true );
	m_ui->createButton->setEnabled( true );
	m_ui->importButton->setEnabled( true );
	m_ui->editButtonTray->setVisible( true );
	m_ui->createButtonTray->setVisible( false );
	m_ui->dialogButtonTray->setEnabled( true );

	m_ui->cardSelector->setCurrentRow( 0 );
	cardSelected( 0 );
}

void ManageSdCardsDialog::beginCreate() {
	m_creating = true;
	
	m_ui->cardSelector->setCurrentRow( 0, QItemSelectionModel::Clear );
	m_ui->cardSelector->setEnabled( false );
	m_ui->createButton->setEnabled( false );
	m_ui->importButton->setEnabled( m_cards.size() == 0 );
	m_ui->editButtonTray->setVisible( false );
	m_ui->createButtonTray->setVisible( true );
	m_ui->dialogButtonTray->setEnabled( m_cards.empty() );
	m_ui->cancelButton->setVisible( !m_cards.empty() );

	m_ui->nameInput->setEnabled( true );
	m_ui->nameInput->setText( generateName().c_str() );
	m_ui->filesystemSelect->setCurrentIndex( (int)SdCardFormat::FAT16 );
	m_ui->sizeSpinner->setRange( 32, 2048 );
	m_ui->sizeSpinner->setValue( 64 );

}

void ManageSdCardsDialog::cardSelected( int cardIndex ) {
	if( m_creating ) return;

	if( (size_t)cardIndex > m_cards.size() ) {
		return;
	}

	const SdCardInfo &card = m_cards[cardIndex];
	m_ui->nameInput->setText( card.name.c_str() );
	m_ui->filesystemSelect->setCurrentIndex( (int)card.format );
	m_ui->sizeSpinner->setValue( (int)(card.numBlocks / 1024u) );
	formatChanged( (int)card.format );

	m_ui->nameInput->setEnabled( false );
	m_ui->editButtonTray->setVisible( true );
	m_ui->createButtonTray->setVisible( false );
	m_ui->browseFilesButton->setEnabled( card.format != SdCardFormat::None );
}


void ManageSdCardsDialog::browseFiles() {
	const string cardName = m_ui->nameInput->text().toStdString();

	const auto cardRef = m_mountedCards.find( cardName );
	if( cardRef != m_mountedCards.end() ) {
		FileExplorer::showFolder( fs::to_path( cardRef->second->path() ) );
		return;
	}

	std::shared_ptr<MountedSdCard> card = SdCardManager::mount( cardName );
	if( card == nullptr ) {
		QMessageBox::critical(
			this,
			tr( "Operation Failed" ),
			tr( "Failed to mount the SD card image. For more detailed error information, go back to the main window and press F7 to view error logs." ),
			QMessageBox::Ok
		);
		return;
	}

	m_mountedCards[cardName] = card;
	FileExplorer::showFolder( fs::to_path( card->path() ) );
}

void ManageSdCardsDialog::cloneCard() {
	
	QInputDialog dialog( this );
	dialog.setWindowIcon( Icon::appIcon() );
	dialog.setWindowTitle( tr( "Clone SD Card" ) );
	dialog.setLabelText( tr( "Enter a name for the copied SD card:" ) );
	dialog.setInputMode( QInputDialog::TextInput );
	dialog.setTextValue( generateName().c_str() );
	dialog.setOkButtonText( tr( "Clone" ) );

	QLineEdit *input = dialog.findChild<QLineEdit*>();
	QDialogButtonBox *buttons = dialog.findChild<QDialogButtonBox*>();

	if( input == nullptr ) {
		logError( "QLineEdit not found in QInputDialog" );
		return;
	}

	if( buttons == nullptr ) {
		logError( "QLineEdit not found in QInputDialog" );
		return;
	}

	QPushButton *okButton = buttons->button( QDialogButtonBox::Ok );
	if( okButton == nullptr ) {
		logError( "QDialogButtonBox::Ok not found in QInputDialog" );
		return;
	}

	input->setMaxLength( 11 );
	okButton->setText( tr( "Clone" ) );
	okButton->setIcon( Icon::clone() );

	connect( input, &QLineEdit::textChanged, [&]() {
		validateName( input->text(), input, okButton, m_cards );
	});

	if( dialog.exec() != (int)QDialog::Accepted ) {
		return;
	}

	SdCardManager::copyCard(
		m_ui->nameInput->text().toStdString(),
		input->text().toStdString()
	);
	
	refreshList();
	discard();

	for( int i = 0; i < m_ui->cardSelector->count(); i++ ) {
		if( m_ui->cardSelector->item( i )->text() == input->text() ) {
			m_ui->cardSelector->setCurrentRow( i );
			cardSelected( i );
			return;
		}
	}
}

bool ManageSdCardsDialog::unmountOne( const std::string &cardName ) {
	auto cardRef = m_mountedCards.find( cardName );
	if( cardRef == m_mountedCards.end() ) return true;

	MountedSdCard &card = *cardRef->second;
	if( card.unmount() ) {
		m_mountedCards.erase( cardName );
		return true;
	}

	DeviceBusyDialog busyDialog( this );
	QTimer busyPoll;

	connect( &busyPoll, &QTimer::timeout, [&](){
		if( !card.unmount() ) return;
		busyPoll.stop();
		busyDialog.accept();
	});

	busyPoll.setInterval( 1000 );
	busyPoll.start();

	if( busyDialog.exec() == QDialog::Accepted ) {
		busyPoll.stop();
		m_mountedCards.erase( cardName );
		return true;
	}

	busyPoll.stop();
	return false;
}

bool ManageSdCardsDialog::tryUnmountAll() {
	HashMap<std::string, std::shared_ptr<MountedSdCard>> busyCards;
	for( const auto &[name,card] : m_mountedCards ) {
		if( !card->unmount() ) busyCards[name] = card;
	}

	m_mountedCards.clear();
	m_mountedCards = std::move( busyCards );

	return m_mountedCards.empty();
}

bool ManageSdCardsDialog::shouldClose() {
	if( m_mountedCards.empty() ) return true;

	if( QMessageBox::information(
		this,
		tr( "Confirm Close" ),
		tr( "All SD cards you have opened for browsing will be closed. Continue?" ),
		QMessageBox::Ok | QMessageBox::Cancel ) != QMessageBox::Ok
	) return false;

	if( tryUnmountAll() ) return true;

	DeviceBusyDialog busyDialog( this );
	QTimer busyPoll;

	connect( &busyPoll, &QTimer::timeout, [&](){
		if( !this->tryUnmountAll() ) return;
		busyPoll.stop();
		busyDialog.accept();
	});

	busyPoll.setInterval( 1000 );
	busyPoll.start();

	if( busyDialog.exec() == QDialog::Accepted ) {
		busyPoll.stop();
		m_mountedCards.clear();
		return true;
	}

	return false;
}

void ManageSdCardsDialog::closeEvent( QCloseEvent *event ) {
	if( shouldClose() ) {
		QDialog::closeEvent( event );
	} else {
		event->ignore();
	}
}

void ManageSdCardsDialog::accept() {
	if( shouldClose() ) QDialog::accept();
}

void ManageSdCardsDialog::reject() {
	if( shouldClose() ) QDialog::reject();
}

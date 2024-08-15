#include "src/ui/import-sd-card-dialog.hpp"
#include "ui_import-sd-card-dialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <stdexcept>
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/core/sdcard.hpp"
#include "src/core/numeric-string.hpp"

#ifdef _WIN32
#include <cctype>

static inline string uppercase( string str ) {
	for( size_t i = 0; i < str.length(); i++ ) {
		str[i] = (char)std::toupper( (int)str[i] );
	}
	return str;
}
#endif

static inline size_t min( size_t a, size_t b ) noexcept {
	return a < b ? a : b;
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

static QString makeSafeName( const QString &name ) {
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

ImportSdCardDialog::ImportSdCardDialog( QWidget *parent, const fs::path &filePath ) :
	QDialog( parent ),
	m_filePath( filePath ),
	m_ui( new Ui::ImportSdCardDialog )
{
	m_ui->setupUi( this );

	setWindowIcon( Icon::appIcon() );
	UiUtil::scaleWindow( this );
	UiUtil::fixDialogButtonsOnWindows( this );
	
	m_ui->buttonTray->button( QDialogButtonBox::Ok )->setText( tr( "Import" ) );

	for( const SdCardInfo &card : SdCardManager::fetchAll() ) {
#ifdef _WIN32
		m_usedNames.insert( uppercase( card.name ) );
#else
		m_usedNames.insert( card.name );
#endif
	}

	const bool canHardLink = SdCardManager::canHardLink( filePath );
	m_ui->hardLinkRadio->setEnabled( canHardLink );
	m_ui->copyFileRadio->setEnabled( canHardLink );

	string name = SdCardManager::tryReadVolumeName( filePath );
	if( name.empty() ) {
		name = filePath.stem().u8string().substr( 0, 11 );
	}

	name = makeSafeName( QString::fromStdString( name ) ).toStdString();
#ifdef _WIN32
	if( !name.empty() && m_usedNames.count( uppercase( name ) ) > 0 ) {
#else
	if( !name.empty() && m_usedNames.count( name ) > 0 ) {
#endif
		size_t baseLength = name.length();
		int64 i = getNumericSuffix( name, baseLength );
		do {
			const string suffix = "~"s + Number::toString( ++i );
			name = name.substr( 0, min( baseLength, (size_t)11 - suffix.length() ) ) + suffix;
		} while(
#ifdef _WIN32
			m_usedNames.count( uppercase( name ) ) > 0
#else
			m_usedNames.count( name ) > 0
#endif
		);
	}

	m_ui->volumeName->setText( name.c_str() );
	nameChanged( m_ui->volumeName->text() );
}

ImportSdCardDialog::~ImportSdCardDialog() {
	delete m_ui;
}

void ImportSdCardDialog::nameChanged( QString name ) {
	const QString safeName = makeSafeName( name );
	if( name != safeName ) m_ui->volumeName->setText( safeName );

	name = safeName.trimmed();
#ifdef _WIN32
	name = name.toUpper();
#endif

	if( name.isEmpty() ) {
		m_ui->errorLabel->clear();
		m_ui->buttonTray->button( QDialogButtonBox::Ok )->setEnabled( false );
		return;
	}

	if( m_usedNames.count( name.toStdString() ) > 0 ) {
		m_ui->errorLabel->setText( tr( "This name is already in use" ) );
		m_ui->buttonTray->button( QDialogButtonBox::Ok )->setEnabled( false );
		return;
	}

#ifdef _WIN32
	if(
		name == QString( "CON" ) ||
		name == QString( "PRN" ) ||
		name == QString( "AUX" ) ||
		name == QString( "NUL" ) ||
		(
			name.length() == 4 &&
			(name.startsWith( "COM" ) || name.startsWith( "LPT" ) ) &&
			name[3].isDigit()
		)
	) {
		m_ui->errorLabel->setText( tr( "This name is reserved by Windows" ) );
		m_ui->buttonTray->button( QDialogButtonBox::Ok )->setEnabled( false );
		return;
	}
#endif

	m_ui->errorLabel->clear();
	m_ui->buttonTray->button( QDialogButtonBox::Ok )->setEnabled( true );
}

void ImportSdCardDialog::accept() {
	if( !SdCardManager::import(
		m_filePath,
		m_ui->volumeName->text().trimmed().toStdString(),
		m_ui->copyFileRadio->isChecked()
	)) {
		QMessageBox::critical( this, tr( "Unexpected Error" ), tr( "Failed to import SD card." ) );
		return;
	}

	QDialog::accept();
}

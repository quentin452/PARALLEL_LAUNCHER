#include "src/rhdc/ui/rhdc-login-dialog.hpp"
#include "ui_rhdc-login-dialog.h"

#include <QMessageBox>
#include "src/polyfill/file-dialog.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/steam-deck.hpp"
#include "src/core/sm64.hpp"
#include "src/core/rom.hpp"
#include "src/db/data-provider.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/rhdc/web/api.hpp"

RhdcLoginDialog::RhdcLoginDialog( QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::RhdcLoginDialog ),
	m_exists( new bool( true ) )
{
	m_ui->setupUi( this );
	m_ui->settingsContainer->setVisible( false );
	m_ui->browseForRomButton->setIcon( Icon::browse() );
	m_ui->downloadDirButton->setIcon( Icon::browse() );
	m_ui->loginButton->setIcon( Icon::login() );
	m_ui->verifyMfaButton->setIcon( Icon::ok() );
	setWindowIcon( Icon::appIcon() );
	UiUtil::fixDialogButtonsOnWindows( this );

	QFont codeFont = m_ui->mfaInput->font();
	codeFont.setLetterSpacing( QFont::PercentageSpacing, 150.0 );
	m_ui->mfaInput->setFont( codeFont );

#ifdef __APPLE__
	UiUtil::fixFontSizeOnMac( m_ui->label_4 );
	UiUtil::fixFontSizeOnMac( m_ui->label_7 );
	UiUtil::fixFontSizeOnMac( m_ui->label_8 );
	UiUtil::fixFontSizeOnMac( m_ui->label_9 );
	UiUtil::fixFontSizeOnMac( m_ui->label_10 );
	UiUtil::fixFontSizeOnMac( m_ui->label_11 );
	UiUtil::fixFontSizeOnMac( m_ui->preferHleLabel );
#endif
}

RhdcLoginDialog::~RhdcLoginDialog() {
	*m_exists = false;
	delete m_ui;
}

void RhdcLoginDialog::showEvent( QShowEvent *event ) {
	fs::path _discard;
	m_ui->pages->setCurrentIndex( SM64::tryGetPath( _discard ) ? 1 : 0 );
	m_ui->usernameInput->clear();
	m_ui->passwordInput->clear();
	m_ui->loginErrorMessage->clear();
	m_ui->buttonTray->button( QDialogButtonBox::Cancel )->setVisible( true );
	m_ui->buttonTray->button( QDialogButtonBox::Ok )->setVisible( false );
	m_ui->loginSpinner->setVisible( false );
	m_ui->loginButton->setEnabled( true );
	m_ui->settingsContainer->setVisible( false );
	QDialog::showEvent( event );

	QRect geo = geometry();
	geo.setWidth( 450 );
	geo.setHeight( 275 );
	setGeometry( geo );

	if( SteamDeck::isSteamDeck() ) {
		m_ui->preferHleCheckbox->setVisible( false );
		m_ui->preferHleLabel->setVisible( false );
	}
}

static const string SM64_SHA1 = "9bef1128717f958171a4afac3ed78ee2bb4e86ce";

void RhdcLoginDialog::browseForRom() {
	const fs::path romPath = FileDialog::getFile(
		tr( "Select SM64 ROM" ).toStdString().c_str(),
		{{ "z64 ROM", "*.z64" }}
	);
	if( romPath.empty() ) return;

	if( Sha1::compute( romPath ) != SM64_SHA1 ) {
		QMessageBox::critical( this, tr( "ROM Does not Match" ), tr( "The provided rom does not match the expected checksum." ) );
		return;
	}

	DataProvider::addManualRomPath( romPath );
	DataProvider::addRomFile({
		romPath,
		RomUtil::getLastModified( romPath ),
		SM64_SHA1,
		true
	});
	DataProvider::addRomInfo({
		SM64_SHA1,
		"Super Mario 64",
		"SUPER MARIO 64",
		EmulatorCore::ParallelN64,
		GfxPlugin::ParaLLEl,
		GfxPlugin::ParaLLEl,
		true,
		true,
		false,
		true,
		false,
		0,
		0,
		std::set<string>(),
		true,
		false,
		1021708041,
		DefaultInputModes::Normal.id,
		WidescreenMode::Off,
		false,
		false,
		{ N64InputType::Unknown, N64InputType::None, N64InputType::None, N64InputType::None },
		""
	});

	m_ui->pages->setCurrentIndex( 1 );
}

void RhdcLoginDialog::browseForDir() {
	const fs::path downloadDir = FileDialog::getDirectory(
		tr( "Select a new download directory" ).toStdString().c_str()
	);
	if( downloadDir.empty() ) return;
	m_ui->downloadDirInput->setText( downloadDir.u8string().c_str() );
}

void RhdcLoginDialog::validateMfaCode() {
	QString value = m_ui->mfaInput->text();
	QString safeValue;

	for( int i = 0; i < value.length() && safeValue.length() < 6; i++ ) {
		if( value[i].isDigit() ) safeValue.append( value[i] );
	}

	if( safeValue != value ) {
		m_ui->mfaInput->setText( safeValue );
	}

	m_ui->verifyMfaButton->setEnabled( safeValue.length() == 6 );
}

void RhdcLoginDialog::login() {
	if( m_ui->usernameInput->text().isEmpty() ) {
		m_ui->loginErrorMessage->setText( tr( "Enter your username" ) );
		return;
	}

	if( m_ui->passwordInput->text().isEmpty() ) {
		m_ui->loginErrorMessage->setText( tr( "Enter your password" ) );
		return;
	}

	m_ui->loginButton->setEnabled( false );
	m_ui->loginSpinner->setVisible( true );
	m_ui->loginErrorMessage->clear();

	std::shared_ptr<bool> dialogExists = m_exists;
	RhdcApi::loginAsync(
		m_ui->usernameInput->text().toStdString(),
		m_ui->passwordInput->text().toStdString(),
		/* mfaCode */ -1,
		/* awaitRetryAfter */ true,
		[this,dialogExists]() {
			if( !*dialogExists ) return;
			this->m_ui->pages->setCurrentIndex( 3 );
			this->m_ui->buttonTray->button( QDialogButtonBox::Cancel )->setVisible( false );
			this->m_ui->buttonTray->button( QDialogButtonBox::Ok )->setVisible( true );

			QRect geo = this->geometry();
			geo.setWidth( 525 );
			geo.setHeight( 575 );
			this->setGeometry( geo );
			this->m_ui->settingsContainer->setVisible( true );

			const RhdcSettings rhdcSettings = FileController::loadRhdcSettings();
			this->m_ui->downloadDirInput->setText( rhdcSettings.downloadDirectory.u8string().c_str() );
			this->m_ui->enableStarDisplayCheckbox->setChecked( rhdcSettings.enableStarDisplay );
			this->m_ui->allSlotsCheckbox->setChecked( rhdcSettings.checkAllSaveSlots );
			this->m_ui->preferHleCheckbox->setChecked( rhdcSettings.preferHle );
			this->m_ui->enableDefaultLayoutsCheckbox->setChecked( rhdcSettings.enableDefaultLayouts );
			this->m_ui->starDisplaySubsection->setEnabled( rhdcSettings.enableStarDisplay );
			this->m_ui->ignoreWidescreenCheckbox->setChecked( rhdcSettings.ignoreWidescreenHint );
		},
		[this,dialogExists](ApiErrorType error) {
			if( !*dialogExists ) return;

			if( error == ApiErrorType::MfaCodeRequired ) {
				this->m_ui->pages->setCurrentIndex( 2 );
				this->m_ui->loginSpinner2->setVisible( false );
				this->m_ui->mfaErrorMessage->setVisible( false );
				this->m_ui->verifyMfaButton->setEnabled( false );
				this->m_ui->mfaInput->clear();
				return;
			}

			this->m_ui->loginSpinner->setVisible( false );
			this->m_ui->loginButton->setEnabled( true );
			switch( error ) {
				case ApiErrorType::NotAuthorized:
					this->m_ui->loginErrorMessage->setText( tr( "Username or password is incorrect" ) );
					break;
				case ApiErrorType::ServerDown:
					this->m_ui->loginErrorMessage->setText( tr( "The romhacking.com servers appear to be down." ) );
					break;
				default:
					this->m_ui->loginErrorMessage->setText( tr( "An unknown error occurred." ) );
					break;
			}
		}
	);
}

void RhdcLoginDialog::sendMfaCode() {
	this->m_ui->loginSpinner2->setVisible( true );
	this->m_ui->verifyMfaButton->setEnabled( false );

	std::shared_ptr<bool> dialogExists = m_exists;
	RhdcApi::loginAsync(
		m_ui->usernameInput->text().toStdString(),
		m_ui->passwordInput->text().toStdString(),
		m_ui->mfaInput->text().toInt( nullptr, 10 ),
		/* awaitRetryAfter */ true,
		[this,dialogExists]() {
			if( !*dialogExists ) return;
			this->m_ui->pages->setCurrentIndex( 3 );
			this->m_ui->buttonTray->button( QDialogButtonBox::Cancel )->setVisible( false );
			this->m_ui->buttonTray->button( QDialogButtonBox::Ok )->setVisible( true );

			QRect geo = this->geometry();
			geo.setWidth( 525 );
			geo.setHeight( 575 );
			this->setGeometry( geo );
			this->m_ui->settingsContainer->setVisible( true );

			const RhdcSettings rhdcSettings = FileController::loadRhdcSettings();
			this->m_ui->downloadDirInput->setText( rhdcSettings.downloadDirectory.u8string().c_str() );
			this->m_ui->enableStarDisplayCheckbox->setChecked( rhdcSettings.enableStarDisplay );
			this->m_ui->allSlotsCheckbox->setChecked( rhdcSettings.checkAllSaveSlots );
			this->m_ui->preferHleCheckbox->setChecked( rhdcSettings.preferHle );
			this->m_ui->enableDefaultLayoutsCheckbox->setChecked( rhdcSettings.enableDefaultLayouts );
			this->m_ui->starDisplaySubsection->setEnabled( rhdcSettings.enableStarDisplay );
			this->m_ui->ignoreWidescreenCheckbox->setChecked( rhdcSettings.ignoreWidescreenHint );
		},
		[this,dialogExists](ApiErrorType) {
			if( !*dialogExists ) return;
			this->m_ui->mfaErrorMessage->setVisible( true );
			this->m_ui->verifyMfaButton->setEnabled( true );
			this->m_ui->loginSpinner2->setVisible( false );
		}
	);
}

void RhdcLoginDialog::accept() {
	FileController::saveRhdcSettings({
		fs::to_path( m_ui->downloadDirInput->text().toStdString() ),
		m_ui->enableStarDisplayCheckbox->isChecked(),
		m_ui->allSlotsCheckbox->isChecked(),
		m_ui->preferHleCheckbox->isChecked(),
		m_ui->enableDefaultLayoutsCheckbox->isChecked(),
		m_ui->ignoreWidescreenCheckbox->isChecked()
	});
	QDialog::accept();
}

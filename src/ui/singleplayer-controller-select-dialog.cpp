#include "src/ui/singleplayer-controller-select-dialog.hpp"
#include "ui_singleplayer-controller-select-dialog.h"

#include "src/ui/util.hpp"
#include "src/ui/icons.hpp"

SingleplayerControllerSelectDialog::SingleplayerControllerSelectDialog() :
	QDialog( nullptr ),
	m_ui( new Ui::SingleplayerControllerSelectDialog ),
	m_gamepad( ConnectedGamepad{ -1, ControllerInfo() } )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
	UiUtil::scaleWindow( this );
	UiUtil::fixDialogButtonsOnWindows( this );

	GamepadController::instance().start();
	connect( &GamepadController::instance(), &GamepadController::gamepadButtonPressed, this, &SingleplayerControllerSelectDialog::buttonPressed );
}

SingleplayerControllerSelectDialog::~SingleplayerControllerSelectDialog() {
	GamepadController::instance().stop();
	delete m_ui;
}

void SingleplayerControllerSelectDialog::buttonPressed( GamepadButtonEvent event ) {
	if( !event.isPressed ) return;
	for( const ConnectedGamepad &gamepad : GamepadController::instance().getConnected() ) {
		if( gamepad.id == event.id ) {
			m_gamepad = gamepad;
			close();
			return;
		}
	}
}

void SingleplayerControllerSelectDialog::keyPressEvent( QKeyEvent *event ) {
	if( event->key() == Qt::Key_Escape ) {
		m_gamepad = ConnectedGamepad{ -1, ControllerInfo() };
		event->accept();
		close();
	} else {
		event->ignore();
	}
}

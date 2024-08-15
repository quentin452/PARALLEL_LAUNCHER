#include "src/ui/bind-input-dialog.hpp"
#include "ui_bind-input-dialog.h"

#include <cmath>
#include <QTimer>
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

static constexpr double AXIS_MOVE_THRESHOLD = 0.2;

BindInputDialog::BindInputDialog( const QString &input, GamepadId gamepadId, bool showSkipButton ) :
	QDialog( nullptr ),
	m_ui( new Ui::BindInputDialog ),
	m_gamepadId( gamepadId ),
	m_binding({ BindingType::None, 0 })
{
	m_ui->setupUi( this );
	UiUtil::scaleWindow( this );

	m_ui->skipButton->setVisible( showSkipButton );
	m_ui->inputName->setText( input );
	setWindowTitle( input );

	setWindowIcon( Icon::appIcon() );
	m_ui->skipButton->setIcon( Icon::skip() );
	m_ui->cancelButton->setIcon( Icon::cancel() );

	UiUtil::fixFontSizeOnMac( m_ui->inputName );

	connect( &GamepadController::instance(), SIGNAL( gamepadButtonPressed(GamepadButtonEvent) ), this, SLOT( buttonPressed(GamepadButtonEvent) ) );
	connect( &GamepadController::instance(), SIGNAL( gamepadAxisMoved(GamepadAxisEvent) ), this, SLOT( axisMoved(GamepadAxisEvent) ) );
	connect( &GamepadController::instance(), SIGNAL( gamepadHatMoved(GamepadHatEvent) ), this, SLOT( hatMoved(GamepadHatEvent) ) );
	m_initialState = GamepadController::instance().getState( gamepadId );
}

BindInputDialog::~BindInputDialog() {
	cleanup();
	delete m_ui;
}

void BindInputDialog::cleanup() {
	for( auto &i : m_axisTimers ) {
		i.second->stop();
		i.second->deleteLater();
	}
	m_axisTimers.clear();
}

void BindInputDialog::buttonPressed( GamepadButtonEvent event ) {
	if( event.id != m_gamepadId || !event.isPressed ) return;

	m_binding = {
		BindingType::Button,
		(ushort)event.button
	};
	cleanup();
	accept();
}

void BindInputDialog::axisMoved( GamepadAxisEvent event ) {
	if( event.id != m_gamepadId ) return;

	if(
		std::abs( event.position ) > AXIS_MOVE_THRESHOLD &&
		std::abs( event.position - m_initialState.axes.at( event.axis ) ) > AXIS_MOVE_THRESHOLD
	) {
		if( m_axisTimers.count( event.axis ) == 0 ) {
			QTimer *timer = new QTimer( this );
			connect( timer, SIGNAL(timeout()), this, SLOT(axisHeld()) );
			timer->setSingleShot( true );
			timer->start( 500 );
			m_axisTimers[event.axis] = timer;
		}
	} else {
		if( m_axisTimers.count( event.axis ) > 0 ) {
			m_axisTimers[event.axis]->stop();
			m_axisTimers[event.axis]->deleteLater();
			m_axisTimers.erase( event.axis );
		}
	}
}

void BindInputDialog::hatMoved( GamepadHatEvent event ) {
	if( event.id != m_gamepadId ) return;

	if( Flags::has( event.position, HatDirection::Up ) ) {
		m_binding = { BindingType::HatUp, event.hat };
	} else if( Flags::has( event.position, HatDirection::Down ) ) {
		m_binding = { BindingType::HatDown, event.hat };
	} else if( Flags::has( event.position, HatDirection::Left ) ) {
		m_binding = { BindingType::HatLeft, event.hat };
	} else if( Flags::has( event.position, HatDirection::Right ) ) {
		m_binding = { BindingType::HatRight, event.hat };
	} else return;

	cleanup();
	accept();
}

void BindInputDialog::axisHeld() {
	for( const auto &i : m_axisTimers ) {
		if( i.second->remainingTime() <= 0 ) {
			const GamepadState currentState = GamepadController::instance().getState( m_gamepadId );
			if( i.first >= currentState.axes.size() ) continue;

			m_binding = {
				currentState.axes.at( i.first ) > 0 ? BindingType::AxisPositive : BindingType::AxisNegative,
				(ushort)i.first
			};

			cleanup();
			accept();
			return;
		}
	}
}

Binding BindInputDialog::getBinding( bool &cancelled ) {
	cancelled = (exec() == QDialog::Rejected);
	return m_binding;
}

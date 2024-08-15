#include "src/ui/keyboard-config-dialog.hpp"
#include "ui_keyboard-config-dialog.h"

#include <QFormLayout>
#include <QLabel>
#include "src/core/file-controller.hpp"
#include "src/core/hotkeys.hpp"
#include "src/ui/hotkey-edit-widget.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/polyfill/crash.hpp"

static constexpr size_t NUM_BINDS = (size_t)Hotkey::NUM_HOTKEYS;
static constexpr size_t NUM_CONTROLS = (size_t)ControllerAction::NUM_INPUTS;
static constexpr size_t NUM_HOTKEYS = NUM_BINDS - NUM_CONTROLS;

static const char *const s_actionNames[] = {
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Analog Up" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Analog Down" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Analog Left" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Analog Right" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "C Up" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "C Down" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "C Left" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "C Right" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "D-Pad Up" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "D-Pad Down" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "D-Pad Left" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "D-Pad Right" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "A Button" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "B Button" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "L Trigger" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Z Trigger" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "R Trigger" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Start Button" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "X Button" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Y Button" ),

	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Save State" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Load State" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Previous State" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Next State" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Previous Cheat" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Next Cheat" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Toggle Cheat" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Toggle FPS Display" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Pause/Unpause" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Frame Advance" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Fast Forward (Hold)" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Fast Forward (Toggle)" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Slow Motion (Hold)" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Slow Motion (Toggle)" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Rewind*" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Quit Emulator" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Hard Reset" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Toggle Fullscreen" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "RetroArch Menu" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Record Video" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Record Inputs" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Take Screenshot" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Raise Volume" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Lower Volume" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Mute/Unmute" ),
	QT_TRANSLATE_NOOP( "KeyboardConfigDialog", "Grab Mouse" )
};
static_assert( sizeof( s_actionNames ) == NUM_BINDS * sizeof(void*) );

KeyboardConfigDialog::KeyboardConfigDialog( QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::KeyboardConfigDialog )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
	UiUtil::scaleWindow( this );
	UiUtil::fixDialogButtonsOnWindows( this );

	QFormLayout *controlsLayout = dynamic_cast<QFormLayout*>( m_ui->controlsArea->layout() );
	QFormLayout *hotkeysLayout = dynamic_cast<QFormLayout*>( m_ui->hotkeysArea->layout() );

	pl_assert( controlsLayout != nullptr );
	for( size_t i = 0; i < NUM_CONTROLS; i++ ) {
		QLabel *label = new QLabel( tr( s_actionNames[i] ) );
		label->setAlignment( Qt::AlignRight | Qt::AlignBottom );
		controlsLayout->addRow(	label, new HotkeyEditPanel( m_ui->controlsArea ) );
	}

	pl_assert( hotkeysLayout != nullptr );
	for( size_t i = 0; i < NUM_HOTKEYS; i++ ) {
		QLabel *label = new QLabel( tr( s_actionNames[i + NUM_CONTROLS] ) );
		label->setAlignment( Qt::AlignRight | Qt::AlignBottom );
		hotkeysLayout->addRow( label, new HotkeyEditPanel( m_ui->hotkeysArea ) );
	}

	QLayoutItem *rewindLabel = hotkeysLayout->itemAt( (int)((size_t)Hotkey::Rewind - NUM_CONTROLS), QFormLayout::LabelRole );
	pl_assert( rewindLabel != nullptr );
	pl_assert( rewindLabel->widget() != nullptr );

	rewindLabel->widget()->setToolTip( tr( "You must enable rewind functionality in the RetroArch menu to use this hotkey." ) );
}

KeyboardConfigDialog::~KeyboardConfigDialog() {
	delete m_ui;
}

static inline HotkeyEditPanel *getPanel( QWidget *container, size_t row ) {
	QFormLayout *layout = dynamic_cast<QFormLayout*>( container->layout() );
	pl_assert( layout != nullptr );
	QLayoutItem *field = layout->itemAt( (int)row, QFormLayout::FieldRole );
	pl_assert( field != nullptr );
	HotkeyEditPanel *panel = dynamic_cast<HotkeyEditPanel*>( field->widget() );
	pl_assert( panel != nullptr );
	return panel;
}

void KeyboardConfigDialog::showEvent( QShowEvent *event ) {
	const std::vector<KeyId> &hotkeys = FileController::loadHotkeys();
	for( size_t i = 0; i < NUM_CONTROLS; i++ ) {
		getPanel( m_ui->controlsArea, i )->setValue( hotkeys[i] );
	}
	for( size_t i = 0; i < NUM_HOTKEYS; i++ ) {
		getPanel( m_ui->hotkeysArea, i )->setValue( hotkeys[NUM_CONTROLS + i] );
	}
	QDialog::showEvent( event );
}

void KeyboardConfigDialog::dialogButtonClicked( QAbstractButton *button ) {
	if( button == m_ui->buttonTray->button( QDialogButtonBox::Apply ) ) {
		std::vector<KeyId> hotkeys;
		hotkeys.reserve( NUM_BINDS );

		for( size_t i = 0; i < NUM_CONTROLS; i++ ) {
			hotkeys.push_back( getPanel( m_ui->controlsArea, i )->value() );
		}

		for( size_t i = 0; i < NUM_HOTKEYS; i++ ) {
			hotkeys.push_back( getPanel( m_ui->hotkeysArea, i )->value() );
		}

		pl_assert( hotkeys.size() == NUM_BINDS );
		FileController::saveHotkeys( hotkeys );
		QDialog::accept();
	} else if( button == m_ui->buttonTray->button( QDialogButtonBox::Discard ) ) {
		QDialog::reject();
	}
}

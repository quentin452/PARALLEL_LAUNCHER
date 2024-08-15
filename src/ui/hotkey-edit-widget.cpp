#include "src/ui/hotkey-edit-widget.hpp"
#include "ui_hotkey-edit-panel.h"

#include <QKeyEvent>
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

static const char *const s_textNone = QT_TRANSLATE_NOOP( "HotkeyEditWidget", "None" );

HotkeyEditWidget::HotkeyEditWidget( QWidget *parent ) :
	QPushButton( parent ),
	m_hotkey( KeyId::INVALID ),
	m_active( false )
{
	connect( this, SIGNAL( clicked() ), this, SLOT( onClick() ) );
	setIcon( Icon::configure() );
	setText( tr( s_textNone ) );
	setStyleSheet( "text-align:left;" );
	setFocusPolicy( Qt::StrongFocus );
}

void HotkeyEditWidget::setValue( KeyId hotkey ) {
	const char *const keyName = Keycode::getNames( hotkey ).displayName;
	const KeyId prevKey = m_hotkey;
	if( keyName[0] == '\0' ) {
		m_hotkey = KeyId::INVALID;
		if( !m_active ) setText( tr( s_textNone ) );
	} else {
		m_hotkey = hotkey;
		if( !m_active ) setText( keyName );
	}

	if( m_hotkey != prevKey ) {
		emit hotkeyChanged( m_hotkey );
	}
}

void HotkeyEditWidget::clear() {
	setValue( KeyId::INVALID );
}

void HotkeyEditWidget::onClick() {
	m_active = true;
	setText( "Input..." );
}

void HotkeyEditWidget::focusOutEvent( QFocusEvent *event ) {
	m_active = false;
	const char *const keyName = Keycode::getNames( m_hotkey ).displayName;
	setText( keyName[0] == '\0' ? tr( s_textNone ) : QString( keyName ) );
	QPushButton::focusOutEvent( event );
}

static inline uint getKeyCode( QKeyEvent *event ) {
#ifdef __APPLE__
	// Can't distinguish between left and right modifier keys on Mac
	switch( event->key() ) {
		case Qt::Key_Shift: return 56;
		case Qt::Key_Meta: return 59; // Actually Ctrl
		case Qt::Key_Alt: return 58;
		case Qt::Key_Control: return 55; // Actually Super/Command
		default: return event->nativeVirtualKey();
	}
#else
	return event->nativeScanCode();
#endif
}

void HotkeyEditWidget::keyPressEvent( QKeyEvent *event ) {
	if( !m_active ) {
		QPushButton::keyPressEvent( event );
		return;
	}

	const KeyId keyCode = Keycode::fromNativeCode( getKeyCode( event ) );
	const char *keyName = Keycode::getNames( keyCode ).displayName;

	if( keyName[0] == '\0' ) {
		QPushButton::keyPressEvent( event );
		return;
	}

	m_active = false;
	setValue( keyCode );
}

bool HotkeyEditWidget::focusNextPrevChild( bool next ) {
	if( m_active ) return false;
	return QPushButton::focusNextPrevChild( next );
}

HotkeyEditPanel::HotkeyEditPanel( QWidget *parent ) :
	QWidget( parent ),
	m_ui( new Ui::HotkeyEditPanel )
{
	m_ui->setupUi( this );
	m_ui->clearButton->setIcon( Icon::clear() );
	UiUtil::setIconSize( m_ui->clearButton, 16 );
	connect( m_ui->hotkeyEdit, SIGNAL( hotkeyChanged( KeyId ) ), this, SIGNAL( hotkeyChanged( KeyId ) ) );
}

HotkeyEditPanel::~HotkeyEditPanel() {
	delete m_ui;
}

KeyId HotkeyEditPanel::value() const {
	return m_ui->hotkeyEdit->value();
}

void HotkeyEditPanel::setValue( KeyId hotkey ) {
	m_ui->hotkeyEdit->setValue( hotkey );
}

void HotkeyEditPanel::clear() {
	m_ui->hotkeyEdit->clear();
}

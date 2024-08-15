#include "src/ui/toast.hpp"

#include <QGraphicsOpacityEffect>
#include <QPalette>
#include <QCursor>
#include <QLabel>
#include <QTimer>
#include <QColor>
#include "src/core/qthread.hpp"

static constexpr int TOAST_LIFETIME = 250;
static constexpr int TOAST_HEIGHT = 50;

static QLabel *s_toast = nullptr;
static QMainWindow *s_window = nullptr;
static QTimer *s_timer = nullptr;

static int s_ticks = 0;

static void deleteToast() {
	if( s_toast != nullptr ) {
		s_toast->hide();
		s_toast->setParent( nullptr );
		s_toast->deleteLater();
		s_toast = nullptr;
	}

	if( s_timer != nullptr ) {
		s_timer->stop();
		s_timer->setParent( nullptr );
		s_timer->deleteLater();
		s_timer = nullptr;
	}
}

static const char *s_cssNormal = "padding: 5px; font-weight: bold;";
static const char *s_cssFade = "padding: 5px; font-weight: bold; opacity: 64;";

static void toastLoop() {
	if( s_toast == nullptr || s_window == nullptr || s_toast->parent() == nullptr || !s_window->isVisible() ) return;
	if( s_ticks++ > TOAST_LIFETIME ) {
		deleteToast();
		return;
	}

	const bool underMouse = s_toast->geometry().contains( QCursor::pos() - s_window->geometry().topLeft() );

	s_toast->graphicsEffect()->setEnabled( underMouse );
	s_toast->setStyleSheet( underMouse ? s_cssFade : s_cssNormal );
	s_toast->setGeometry(
		20,
		s_window->height() - 20 - TOAST_HEIGHT,
		s_window->width() - 40,
		TOAST_HEIGHT
	);
}

void ToastMessageManager::setWindow( QMainWindow *window ) {
	deleteToast();
	if( window != nullptr ) {
		s_window = window;
		QObject::connect( window, &QObject::destroyed, window, [](QObject*){
			deleteToast();
			s_window = nullptr;
		});
	}
}

static inline QColor getColour( ToastType type ) {
	switch( type ) {
		case ToastType::Error: return QColor( 0xC80000 );
		case ToastType::Warn: return QColor( 0xE8AF00 );
		case ToastType::Info: return QColor( 0x0000C8 );
		default: return QColor( 0x00C800 );
	}
}

void ToastMessageManager::display( ToastType type, const QString &message ) {
	QtThread::execOnUiThread( [=](){
		if( s_window == nullptr ) return;
		deleteToast();

		s_toast = new QLabel( message, s_window );
		s_toast->setAutoFillBackground( true );
		s_toast->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
		s_toast->setTextFormat( Qt::PlainText );
		s_toast->setTextInteractionFlags( Qt::NoTextInteraction );
		s_toast->setWordWrap( true );
		s_toast->setFocusPolicy( Qt::NoFocus );
		s_toast->setAttribute( Qt::WA_TransparentForMouseEvents, true );

		QPalette toastPalette = s_toast->palette();
		toastPalette.setColor( s_toast->backgroundRole(), getColour( type ) );
		toastPalette.setColor( s_toast->foregroundRole(), QColor( 0xFFFFFF ) );
		s_toast->setPalette( toastPalette );

		QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect();
		opacityEffect->setOpacity( 0.25 );
		s_toast->setGraphicsEffect( opacityEffect );

		s_ticks = 0;
		s_timer = new QTimer( s_window );
		s_timer->setInterval( 16 );
		QObject::connect( s_timer, &QTimer::timeout, s_timer, toastLoop );
		s_timer->start();

		toastLoop();
		s_toast->show();
	});
}


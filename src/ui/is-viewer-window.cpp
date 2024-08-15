#include "src/ui/is-viewer-window.hpp"

#include <QFont>
#include <QLayout>
#include <QFontMetricsF>
#include <QPlainTextEdit>
#include "src/core/file-controller.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

IsViewerWindow::IsViewerWindow() : QMainWindow() {
	setWindowTitle( "IS Viewer" );
	setBaseSize( 400, 600 );
	resize( 400, 600 );
	setWindowIcon( Icon::appIcon() );
	UiUtil::scaleWindow( this );

	QPlainTextEdit *view = new QPlainTextEdit( this );
	view->setReadOnly( true );
	view->setMaximumBlockCount( FileController::loadAppSettings().isViewerHistorySize );
#ifdef __APPLE__
	view->setFont( QFont( "monospace", 13 ) );
#else
	view->setFont( QFont( "Noto Mono", 10 ) );
#endif
	view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
	view->setTabStopDistance( 4.0 * QFontMetricsF( view->font() ).width( ' ' ) );

	setCentralWidget( view );
}

IsViewerWindow::~IsViewerWindow() {
	centralWidget()->deleteLater();
}

void IsViewerWindow::logMessage( QString message ) {
	while( message.startsWith( '\n' ) ) message.remove( 0, 1 );
	while( message.endsWith( '\n' ) ) message.chop( 1 );
	if( message.isEmpty() ) return;

	QPlainTextEdit *view = static_cast<QPlainTextEdit*>( centralWidget() );

	static QBrush c1 = view->palette().brush( QPalette::Base );
	static QBrush c2 = view->palette().brush( QPalette::AlternateBase );

	view->appendPlainText( message );
	
	QTextCursor cursor = view->textCursor();
	cursor.movePosition( QTextCursor::End );
	
	QTextBlockFormat format = cursor.blockFormat();
	format.setBackground( (cursor.blockNumber() % 2) ? c2 : c1 );
	cursor.setBlockFormat( format );
}

void IsViewerWindow::error( bool connected ) {
	const QString message = connected ?
		tr( "Error: IS Viewer was disconnected from the emulator." ) :
		tr( "Error: Failed to connect IS Viewer to the emulator." );

	QPlainTextEdit *view = static_cast<QPlainTextEdit*>( centralWidget() );
	view->appendPlainText( message );

	QTextCursor cursor = view->textCursor();
	cursor.movePosition( QTextCursor::End );
	
	QTextBlockFormat format = cursor.blockFormat();
	format.setBackground( (cursor.blockNumber() % 2) ? view->palette().brush( QPalette::AlternateBase ) : view->palette().brush( QPalette::Base ) );
	format.setForeground( QBrush( Qt::red ) );
	cursor.setBlockFormat( format );
}

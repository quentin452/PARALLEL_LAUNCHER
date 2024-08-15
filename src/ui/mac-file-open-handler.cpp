#include "src/ui/mac-file-open-handler.hpp"

#include <QApplication>
#include <QTimer>
#include "src/rhdc/ui/rhdc-download-dialog.hpp"
#include "src/ui/direct-play-window.hpp"
#include "src/ui/main-window.hpp"
#include "src/ui/play.hpp"
#include "src/db/data-provider.hpp"

bool MacFileOpenHandler::eventFilter( QObject *object, QEvent *event ) {
	if( event->type() != QEvent::FileOpen ) {
		return QObject::eventFilter( object, event );
	}

	const QFileOpenEvent *fileOpenEvent = dynamic_cast<const QFileOpenEvent*>( event );
	if( fileOpenEvent != nullptr ) {
		if( m_path != nullptr ) {
			handleDuringStartup( *fileOpenEvent );
		} else {
			handleAfterStartup( *fileOpenEvent );
		}
	}

	return QObject::eventFilter( object, event );
}

void MacFileOpenHandler::handleDuringStartup( const QFileOpenEvent &event ) const {
	string romPath = event.url().toString().toStdString();
	if( romPath.substr( 0, 7 ) == "file://" ) {
		romPath = romPath.substr( 7 );
	}
	*m_path = fs::path( romPath );
}

void MacFileOpenHandler::handleAfterStartup( const QFileOpenEvent &event ) const {
	MainWindow *mainWindow = nullptr;
	for( QWidget *window : QApplication::topLevelWidgets() ) {
		if( !window->isWindow() ) continue;
		if( dynamic_cast<const QMainWindow*>( window ) == nullptr ) return;
		mainWindow = dynamic_cast<MainWindow*>( window );
		if( mainWindow != nullptr ) break;
	}

	if( mainWindow == nullptr || mainWindow->isHidden() ) return;

	const string path = event.url().toString().toStdString();
	fs::path romPath;

	if( path.substr( 0, 7 ) == "file://" ) {
		romPath = fs::to_path( path.substr( 7 ) );
	} else if( path.substr( 0,  7 ) == "rhdc://" ) {
		romPath = RhdcDownloadDialog::runForRhdcUrl( path );
	} else {
		romPath = fs::to_path( path );
	}

	if( romPath.empty() ) return;

	RomFile romFile;
	RomInfo romInfo;
	bool firstRun;

	if( !Game::tryLoadRom( romPath, romInfo, romFile, firstRun ) ) {
		return;
	}

	const QRect winGeo = mainWindow->geometry();
	mainWindow->hide();

	if( firstRun ) {
		DirectPlayWindow setupWindow( romFile );
		QObject::connect( &setupWindow, &DirectPlayWindow::play, [&](){
			if( !RomUtil::coveredBySearchPath( romPath ) ) {
				DataProvider::addManualRomPath( romPath );
			}

			romInfo = setupWindow.getRomInfo();
			DataProvider::addRomInfo( romInfo );
			DataProvider::addRomFile( romFile );
			Game::play( romFile, romInfo, false, false, [=]() {
				mainWindow->setGeometry( winGeo );
				mainWindow->show();
				QTimer::singleShot( 50, [=](){ mainWindow->setGeometry( winGeo ); } );
				mainWindow->refetchAll();
			});
		});
	} else {
		Game::play( romFile, romInfo, false, false, [=]() {
			mainWindow->setGeometry( winGeo );
			mainWindow->show();
			QTimer::singleShot( 50, [=](){ mainWindow->setGeometry( winGeo ); } );
			mainWindow->refetchAll();
		});
	}
}

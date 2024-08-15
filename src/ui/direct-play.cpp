#include "src/ui/direct-play.hpp"

#include <QCoreApplication>
#include <cstdlib>
#include "src/ui/direct-play-window.hpp"
#include "src/ui/main-window.hpp"
#include "src/ui/play.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/updates.hpp"
#include "src/db/data-provider.hpp"
#include "src/rhdc/core/rom-plugin-updater.hpp"
#include "src/rhdc/web/api.hpp"

static void playRom( const RomFile &romFile, const RomInfo &romInfo ) {
	if( !Game::play( romFile, romInfo, false, true, []() {
#ifndef FLATPAK_VERSION
		RetroUpdater::checkForUpdates( true, false );
#endif
		std::exit( 0 );
	})) std::exit( 1 );
}

static int runWithoutRom( QApplication &app ) {
#ifndef FLATPAK_VERSION
	RetroUpdater::checkForUpdates( false, false );
#endif
	QGuiApplication::setQuitOnLastWindowClosed( false );
	MainWindow mainWindow;
	mainWindow.show();
	return app.exec();
}

static RomInfo makeBasicRomInfo( const string &sha1, const fs::path &romPath ) {
	const AppSettings &settings = FileController::loadAppSettings();
	return RomInfo{
		sha1,
		"",
		RomUtil::getInternalName( romPath ),
		EmulatorCore::UseDefault,
		GfxPlugin::UseDefault,
		GfxPlugin::UseDefault,
		settings.parallelTexRectUpscaling,
		settings.parallelRemoveBorders,
		settings.glidenFramebufferEmulation,
		settings.glidenCorrectDepthCompare,
		false,
		0,
		0,
		std::set<string>(),
		true,
		false,
		RomUtil::getCrc32( romPath ),
		DefaultInputModes::Normal.id,
		WidescreenMode::Off,
		false,
		false,
		RomUtil::readControllerTypes( romPath ),
		""
	};
}

int DirectPlay::start( QApplication &app, fs::path romPath ) {
	RomInfo romInfo;
	RomFile romFile;
	bool firstRun;

	if( !Game::tryLoadRom( romPath, romInfo, romFile, firstRun ) ) {
		return runWithoutRom( app );
	}

	romPath = romFile.path;
	if( firstRun || DataProvider::hasPendingPluginCheck( romFile.sha1 ) ) {
		bool pluginChecked = false;

		const string sha1 = romFile.sha1;
		RhdcApi::getPluginSettings( romFile.sha1, [&](const std::optional<RhdcSettingHintsExt> &options){
			pluginChecked = true;
			if( options.has_value() ) {
				if( firstRun ) {
					romInfo = makeBasicRomInfo( sha1, romPath );
					DataProvider::addRomFile( romFile );
					DataProvider::addRomInfo( romInfo );

					if( !RomUtil::coveredBySearchPath( romPath ) ) {
						DataProvider::addManualRomPath( romPath );
					}
				}
				RomPluginUpdater::applyRhdcRecommendations( options.value() );
				DataProvider::removePendingPluginCheck( options.value().sha1 );
				DataProvider::tryFetchRomByHash( sha1, true, &romInfo );
				firstRun = false;
			} else {
				DataProvider::removePendingPluginCheck( sha1 );
			}
		}, [sha1,&pluginChecked](ApiErrorType){
			pluginChecked = true;
			DataProvider::addPendingPluginCheck( sha1, true );
		}, 1500 );

		while( !pluginChecked ) {
			QCoreApplication::processEvents( QEventLoop::WaitForMoreEvents | QEventLoop::ExcludeUserInputEvents	);
		}
	}

	if( !firstRun ) {
		playRom( romFile, romInfo );
		return app.exec();
	}

	DirectPlayWindow mainWindow( romFile );
	QObject::connect( &mainWindow, &DirectPlayWindow::play, [&](){
		if( !RomUtil::coveredBySearchPath( romPath ) ) {
			DataProvider::addManualRomPath( romPath );
		}

		romInfo = mainWindow.getRomInfo();
		DataProvider::addRomInfo( romInfo );
		DataProvider::addRomFile( romFile );
		playRom( romFile, romInfo );
	});

	mainWindow.show();
	QRect geo = mainWindow.geometry();
	geo.setHeight( 0 );
	mainWindow.setGeometry( geo );
	return app.exec();
}

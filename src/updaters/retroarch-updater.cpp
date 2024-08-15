#include "src/updaters/retroarch-updater.hpp"

#if !defined(__APPLE__) && !defined(FLATPAK_VERSION)
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <functional>
#include <memory>
#include <thread>
#include "src/core/qthread.hpp"
#include "src/core/version.hpp"
#include "src/core/updates.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/retroarch-sdl-fix.hpp"
#include "src/core/zip.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/process.hpp"
#include "src/ui/download-dialog.hpp"

static void headAsync(
	QNetworkAccessManager *web,
	const string &url,
	const std::function<void(bool)> &callback
) {
	QNetworkReply *response = web->head( QNetworkRequest( QUrl( url.c_str() ) ) );

	QObject::connect( response, &QNetworkReply::finished, web, [=](){
		if( response->error() != QNetworkReply::NoError ) {
			callback( false );
			return;
		}

		bool hasStatusCode = false;
		const int statusCode = response->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt( &hasStatusCode );
		response->deleteLater();
		callback( hasStatusCode && statusCode == 200 );
	}, Qt::QueuedConnection );
}

enum VersionMarch {
	Patch = 0,
	Minor = 1,
	Major = 2
};

static inline string buildDownloadUrl( const Version &version ) {
#ifdef __linux__
	return "https://buildbot.libretro.com/stable/"s + version.toString() + "/linux/x86_64/RetroArch.7z";
#else
	#ifdef _WIN64
		return "https://buildbot.libretro.com/stable/"s + version.toString() + "/windows/x86_64/RetroArch.7z";
	#else
		return "https://buildbot.libretro.com/stable/"s + version.toString() + "/windows/x86/RetroArch.7z";
	#endif
#endif
}

static inline Version marchVersion(
	Version version,
	VersionMarch versionMarch
) {
	switch( versionMarch ) {
		case VersionMarch::Patch: version.patch++; break;
		case VersionMarch::Minor: version.minor++; version.patch = 0; break;
		case VersionMarch::Major: version.major++; version.minor = 0; version.patch = 0; break;
		default: break;
	}
	return version;
}

static const Version EARLIEST_VERSION = Version{ 1, 16, 0 };

static void getDownloadHelper(
	QNetworkAccessManager *web,
	Version lastFoundVersion,
	Version earliestVersion,
	VersionMarch versionMarch,
	std::shared_ptr<bool> cancelled,
	std::function<void(Version)> callback
) {
	if( *cancelled ) {
		callback( EARLIEST_VERSION );
		return;
	}

	const string url = buildDownloadUrl( earliestVersion ).c_str();
	headAsync(
		web,
		url,
		[=](bool found) {
			if( found ) {
				getDownloadHelper( web, earliestVersion, marchVersion( earliestVersion, VersionMarch::Patch ), VersionMarch::Patch, cancelled, callback );
			} else if( versionMarch == VersionMarch::Major ) {
				callback( lastFoundVersion );
			} else {
				const VersionMarch newVersionMarch = (VersionMarch)((int)versionMarch + 1);
				getDownloadHelper( web, lastFoundVersion, marchVersion( earliestVersion, newVersionMarch ), newVersionMarch, cancelled, callback );
			}
		}
	);
}

static inline Version getLatestVersion(
	const Version &earliestVersion
) {
	QProgressDialog dialog;
	dialog.setRange( 0, 0 );
	dialog.setLabelText( QCoreApplication::translate( "WindowsRetroArchUpdater", "Checking for RetroArch Updates" ) );

	std::shared_ptr<bool> cancelled( new bool( false ) );
	Version latestVersion = earliestVersion;

	QNetworkAccessManager *web = new QNetworkAccessManager();
	getDownloadHelper( web, earliestVersion, earliestVersion, VersionMarch::Patch, cancelled, [cancelled,&latestVersion,&dialog](Version foundVersion){
		if( *cancelled ) return;
		latestVersion = foundVersion;
		dialog.close();
	});

	dialog.exec();
	*cancelled = true;
	web->deleteLater();
	return latestVersion;
}

#if defined(__linux__)
#define RA_ROOT "RetroArch-Linux-x86_64"
#elif defined(_WIN64)
#define RA_ROOT "RetroArch-Win64"
#else
#define RA_ROOT "RetroArch-Win32"
#endif

static void installWorker(
	QProgressDialog *dialog,
	std::shared_ptr<bool> cancelled,
	bool *result
) {
	const fs::path installDir = RetroArch::getExePath().parent_path();

	fs::error_code err;
	fs::remove_all( installDir, err );
	fs::create_directories( installDir, err );

	AsyncProcess extraction(
#ifdef _WIN32
		(BaseDir::program() / L"7za.exe").u8string(),
#else
		"7zr",
#endif
		{
			"x", (BaseDir::temp() / _NFS("retroarch.7z")).u8string(),
			"-t7z",
			"-aoa",
			"-y",
			"-o"s + installDir.u8string()
		}
	);
	const uint exitCode = extraction.join();

	if( exitCode == 0 ) {
		if( fs::isDirectorySafe( installDir / RA_ROOT ) ) {
			const fs::path tempDir = installDir.parent_path() / _NFS("retroarch-temp");
			fs::remove_all( tempDir, err );
			fs::rename( installDir / RA_ROOT, tempDir, err );
			fs::remove_all( installDir, err );
			fs::rename( tempDir, installDir, err );

#ifdef __linux__
			const fs::path retroData = BaseDir::data() / "retro-data";
			const fs::path tempData = installDir / "RetroArch-Linux-x86_64.AppImage.home" / ".config" / "retroarch";
			fs::remove_all( retroData / "assets", err );
			fs::rename( tempData / "assets", retroData / "assets" );
			fs::remove_all( retroData / "filters", err );
			fs::rename( tempData / "filters", retroData / "filters" );
			fs::remove_all( retroData / "overlays", err );
			fs::rename( tempData / "overlays", retroData / "overlays" );
			fs::remove_all( retroData / "shaders", err );
			fs::rename( tempData / "shaders", retroData / "shaders" );
			fs::remove_all( installDir / "RetroArch-Linux-x86_64.AppImage.home" );
#endif
		}
	}

	QtThread::safeAsync( [=](){
		if( *cancelled ) return;
		*result = (exitCode == 0 && fs::existsSafe( RetroArch::getExePath() ));
		dialog->close();
	});
}

static inline bool downloadAndInstall( const Version &version ) {
	const fs::path tempBundlePath = BaseDir::temp() / _NFS("retroarch.7z");

	fs::error_code err;
	fs::remove( tempBundlePath, err );

	DownloadResult status = DownloadDialog::download(
		QT_TRANSLATE_NOOP( "DownloadDialog", "Downloading RetroArch" ),
		buildDownloadUrl( version ),
		tempBundlePath
	);

	if( !status.success ) {
		QMessageBox::critical(
			nullptr,
			QCoreApplication::translate( "WindowsRetroArchUpdater", "Download Failed" ),
			QCoreApplication::translate( "WindowsRetroArchUpdater", "Failed to download RetroArch." ).append( '\n' ).append( status.errorMessage.c_str() )
		);
		return false;
	}

	QProgressDialog dialog;
	dialog.setRange( 0, 0 );
	dialog.setLabelText( QCoreApplication::translate( "WindowsRetroArchUpdater", "Installing RetroArch" ) );

	bool success = false;
	std::shared_ptr<bool> cancelled( new bool( false ) );

	std::thread( installWorker, &dialog, cancelled, &success ).detach();

	dialog.exec();
	*cancelled = true;

	if( !success ) {
		fs::error_code err;
		fs::remove_all( RetroArch::getExePath().parent_path(), err );

		QMessageBox::critical(
			nullptr,
			QCoreApplication::translate( "WindowsRetroArchUpdater", "Installation Error" ),
			QCoreApplication::translate( "WindowsRetroArchUpdater", "An error occurred attempting to uncompress the portable RetroArch bundle" )
		);
	}

	fs::remove( tempBundlePath, err );
	return success;
}

bool RetroArchUpdater::install() {
	const Version latestVersion = getLatestVersion( EARLIEST_VERSION );
	if( latestVersion < EARLIEST_VERSION ) {
		return false;
	}

	if( downloadAndInstall( latestVersion ) ) {
		InstalledVersionsInfo versions = FileController::loadInstalledVersions();
		versions.retroArchVersion.version = latestVersion;
		FileController::saveInstalledVersions( versions );
#ifdef _WIN32
		RetroArch::fixSdlLibrary();
#endif
		return true;
	}

	return false;
}

void RetroArchUpdater::update() {
	InstalledVersionsInfo versions = FileController::loadInstalledVersions();
	Version &version = versions.retroArchVersion.version;
	if( version < EARLIEST_VERSION ) {
		version = EARLIEST_VERSION;
	}

	Version latestVersion = getLatestVersion( version );
	if( latestVersion <= version ) {
		return;
	}

	if( QMessageBox::question(
		nullptr,
		QCoreApplication::translate( "WindowsRetroArchUpdater", "Install Update?" ),
		QCoreApplication::translate( "WindowsRetroArchUpdater", "An update for RetroArch is available. Would you like to install it now?" )
	) != QMessageBox::Yes ) {
		return;
	}

	version = latestVersion;
	if( downloadAndInstall( latestVersion ) ) {
		FileController::saveInstalledVersions( versions );
	}

#ifdef _WIN32
	RetroArch::fixSdlLibrary();
#endif
}
#endif

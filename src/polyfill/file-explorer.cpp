#include "src/polyfill/file-explorer.hpp"

#include "src/polyfill/process.hpp"

#if defined(_WIN32)

void FileExplorer::showFile( const fs::path &filePath ) {
	AsyncProcess( "explorer.exe /select," + AsyncProcess::quoteAndEscapeArg( filePath.u8string() ) ).detach();
}

void FileExplorer::showFolder( const fs::path &folderPath ) {
	AsyncProcess( "explorer.exe", { folderPath.u8string() } ).detach();
}

#elif defined(__APPLE__)

void FileExplorer::showFile( const fs::path &filePath ) {
	AsyncProcess( "open", { "-R", filePath.u8string() } ).detach();
}

void FileExplorer::showFolder( const fs::path &folderPath ) {
	AsyncProcess( "open", { folderPath.u8string() } ).detach();
}

#else

#include <QDBusConnection>
#include <QDBusMessage>
#include "src/core/logging.hpp"

#ifdef FLATPAK_VERSION
#include <QDBusUnixFileDescriptor>
#include <fcntl.h>
#include <unistd.h>
#endif

void FileExplorer::showFile( const fs::path &filePath ) {

	QDBusConnection connection = QDBusConnection::sessionBus();
	
#ifdef FLATPAK_VERSION
	const int fd = open( filePath.c_str(), O_RDONLY );
	if( fd < 0 ) {
		FileExplorer::showFolder( filePath.parent_path() );
		return;
	}

	QDBusMessage message = QDBusMessage::createMethodCall(
		"org.freedesktop.portal.Desktop",
		"/org/freedesktop/portal/desktop",
		"org.freedesktop.portal.OpenURI",
		"OpenDirectory"
	);

	message.setArguments({
		QVariant( QString() ),
		QVariant::fromValue( QDBusUnixFileDescriptor( fd ) ),
		QVariant( QMap<QString, QVariant>() )
	});

#else
	QDBusMessage message = QDBusMessage::createMethodCall(
		"org.freedesktop.FileManager1",
		"/org/freedesktop/FileManager1",
		"org.freedesktop.FileManager1",
		"ShowItems"
	);

	message.setArguments({
		QVariant( QList<QString>({ QString::fromStdString( filePath.string() ) })),
		QVariant( QString() )
	});
#endif

	QDBusMessage reply = connection.call( message, QDBus::Block );
	if( reply.type() == QDBusMessage::ErrorMessage ) {
		logWarn( "[DBus] "s + reply.errorMessage().toStdString() );
		FileExplorer::showFolder( filePath.parent_path() );
	}

#ifdef FLATPAK_VERSION
	close( fd );
#endif
}

void FileExplorer::showFolder( const fs::path &folderPath ) {
	AsyncProcess( "xdg-open", { folderPath.u8string() } ).detach();
}

#endif

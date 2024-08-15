#ifdef __linux__

#include "src/core/sdcard.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusUnixFileDescriptor>
#include <QDBusObjectPath>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <ios>
#include "src/polyfill/base-directory.hpp"
#include "src/core/logging.hpp"

static bool tryUnmount(
	const QDBusConnection &connection,
	const QString &loopObjectPath,
	bool force
) {
	QDBusMessage message = QDBusMessage::createMethodCall(
		"org.freedesktop.UDisks2",
		loopObjectPath,
		"org.freedesktop.UDisks2.Filesystem",
		"Unmount"
	);

	message.setArguments({
		QVariant( QMap<QString, QVariant>{
			{ QString( "auth.no_user_interaction" ), QVariant( true ) },
			{ QString( "force" ), QVariant( force ) }
		})
	});

	QDBusMessage reply = connection.call( message, QDBus::Block );
	if( reply.type() == QDBusMessage::ErrorMessage ) {
		if( !force && reply.errorName() == "org.freedesktop.UDisks2.Error.DeviceBusy" ) return false;
		logWarn( "[DBus] "s + reply.errorMessage().toStdString() );
	}

	return true;
}

static void loopDelete(
	const QDBusConnection &connection,
	const QString &loopObjectPath
) {
	QDBusMessage message = QDBusMessage::createMethodCall(
		"org.freedesktop.UDisks2",
		loopObjectPath,
		"org.freedesktop.UDisks2.Loop",
		"Delete"
	);

	message.setArguments({
		QVariant( QMap<QString, QVariant>{
			{ QString( "auth.no_user_interaction" ), QVariant( true ) }
		})
	});

	QDBusMessage reply = connection.call( message, QDBus::Block );
	if( reply.type() == QDBusMessage::ErrorMessage ) {
		logWarn( "[DBus] "s + reply.errorMessage().toStdString() );
	}
}

bool MountedSdCard::unmount() {
	if( !m_mounted ) return true;
	const QDBusConnection connection = QDBusConnection::systemBus();
	m_mounted = !tryUnmount( connection, QString::fromStdString( m_device ), false );
	return !m_mounted;
}

MountedSdCard::~MountedSdCard() {
	const QDBusConnection connection = QDBusConnection::systemBus();

	if( m_mounted ) tryUnmount( connection, QString::fromStdString( m_device ), true );
	loopDelete( connection, QString::fromStdString( m_device ) );
	::close( m_fd );
}

// The modifications that need to be done to the SD card images to make them work on Windows
// can cause them to break here. Check if Windows modifications are present, and if so, try
// mounting with them removed
static std::shared_ptr<MountedSdCard> retryMountWithoutWindowsPatch( const std::string &name ) {
	const fs::path imagePath = BaseDir::data() / "sdcard" / fs::to_path(name + ".iso");

	std::fstream image( imagePath, std::ios_base::in | std::ios_base::out | std::ios_base::binary );

	char windowsPatch[16];
	image.seekg( 0x1BE, std::ios_base::beg );
	image.read( windowsPatch, 16 );
	if( !image.good() || windowsPatch[0] != (char)0x80 ) return nullptr;

	image.seekp( 0x1BE, std::ios_base::beg );
	for( int i = 0; i < 16; i++ ) image.put( '\0' );
	image.flush();

	if( !image.good() ) return nullptr;
	image.close();

	std::shared_ptr<MountedSdCard> result = SdCardManager::mount( name );
	if( result == nullptr ) {
		image.clear();
		image.open( imagePath, std::ios_base::out | std::ios_base::binary );
		image.seekp( 0x1BE, std::ios_base::beg );
		image.write( windowsPatch, 16 );
		image.flush();
		image.close();
	}

	return result;
}


std::shared_ptr<MountedSdCard> SdCardManager::mount( const std::string &name ) {
	const fs::path imagePath = BaseDir::data() / "sdcard" / (name + ".iso");
	
	const int fd = ::open( imagePath.c_str(), O_LARGEFILE | O_RDWR );
	if( fd <= 0 ) {
		logWarn( "Failed to open SD card image file" );
		return nullptr;
	}

	const QDBusConnection connection = QDBusConnection::systemBus();

	QDBusMessage message = QDBusMessage::createMethodCall(
		"org.freedesktop.UDisks2",
		"/org/freedesktop/UDisks2/Manager",
		"org.freedesktop.UDisks2.Manager",
		"LoopSetup"
	);

	message.setArguments({
		QVariant::fromValue( QDBusUnixFileDescriptor( fd ) ),
		QVariant( QMap<QString, QVariant>{
			{ QString( "auth.no_user_interaction" ), QVariant( true ) }
		})
	});

	QDBusMessage reply = connection.call( message, QDBus::Block );
	if( reply.type() == QDBusMessage::ErrorMessage ) {
		logWarn( "[DBus] "s + reply.errorMessage().toStdString() );
		::close( fd );
		return nullptr;
	}

	if( reply.arguments().length() != 1 ) {
		logWarn( "[DBus] Unexpected number of return values" );
		::close( fd );
		return nullptr;
	}

	const QDBusObjectPath block = reply.arguments().at( 0 ).value<QDBusObjectPath>();

	message = QDBusMessage::createMethodCall(
		"org.freedesktop.UDisks2",
		block.path(),
		"org.freedesktop.UDisks2.Filesystem",
		"Mount"
	);

	message.setArguments({
		QVariant( QMap<QString, QVariant>{
			{ QString( "auth.no_user_interaction" ), QVariant( true ) },
			{ QString( "fstype" ), QVariant( QString( "vfat" ) ) }
		})
	});

	reply = connection.call( message, QDBus::Block );
	if( reply.type() == QDBusMessage::ErrorMessage ) {
		logWarn( "[DBus] "s + reply.errorMessage().toStdString() );
		loopDelete( connection, block.path() );
		::close( fd );
		return retryMountWithoutWindowsPatch( name );
	}

	if( reply.arguments().length() != 1 ) {
		logWarn( "[DBus] Unexpected number of return values" );
		loopDelete( connection, block.path() );
		::close( fd );
		return nullptr;
	}

	return std::make_shared<MountedSdCard>(
		reply.arguments().at( 0 ).value<QString>().toStdString(),
		block.path().toStdString(),
		fd
	);
}

#endif
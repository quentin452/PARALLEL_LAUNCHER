#include "src/core/is-viewer.hpp"

#include "src/polyfill/base-directory.hpp"
#include "src/core/logging.hpp"
#include "src/core/buffer.hpp"
#include "src/core/uuid.hpp"

using namespace __ISViewer_Private;

void ISViewerThread::run() {
	Buffer message( 0xFFE0 );
	message.data()[0] = '\0';

	std::error_code err;
	if( !m_pipe->connect( err ) ) {
		if( err == std::make_error_code( std::errc::operation_canceled ) ) return;
		logWarn( "Read error attempting to get the first byte from the IS Viewer pipe:"s + err.message() );
		emit error( false );
		return;
	}

	while( m_pipe->active() ) {
		ubyte sizeData[2];
		if( !m_pipe->read( 2, sizeData, err ) ) {
			if( err == std::make_error_code( std::errc::operation_canceled ) ) return;
			emit error( true );
			return;
		}

		if( !m_pipe->active() ) return;

		const ushort messageSize = ((ushort)sizeData[0] << 8) | (ushort)sizeData[1];
		if( messageSize > 0xFFE0 ) {
			logWarn( "Read error in IS Viewer pipe:"s + err.message() );
			emit error( true );
			return;
		}

		if( messageSize == 0 ) continue;
		if( !m_pipe->read( messageSize, message.data(), err ) ) {
			if( err == std::make_error_code( std::errc::operation_canceled ) ) return;
			emit error( true );
			return;
		}

		if( !m_pipe->active() ) return;
		emit messageReceived( QString::fromUtf8( message.data(), messageSize ) );
	}
}

int ISViewer::start() {
	if( m_pipe.active() ) return 1;

	std::error_code err;
	if( !m_pipe.open( Uuid::Random().toString(), err ) ) {
		logError( "Failed to create IS Viewer pipe: "s + err.message() );
		emit error( false );
		return err.value();
	}

	m_thread = new ISViewerThread( this, &m_pipe );
	connect( (ISViewerThread*)m_thread, &ISViewerThread::messageReceived, this, &ISViewer::messageReceived );
	connect( (ISViewerThread*)m_thread, &ISViewerThread::error, this, &ISViewer::error );

	m_thread->start();
	return 0;
}

void ISViewer::stop() {
	m_pipe.close();

	if( !m_thread ) return;
	m_thread->wait();
	m_thread->deleteLater();
	m_thread = nullptr;
}

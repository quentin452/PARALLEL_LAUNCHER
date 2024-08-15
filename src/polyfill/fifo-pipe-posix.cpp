#ifndef _WIN32

#include "src/polyfill/fifo-pipe.hpp"

#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "src/polyfill/base-directory.hpp"

using namespace std::chrono_literals;

/* Input */

ReadableFifoPipe::ReadableFifoPipe() noexcept : m_filePath( "" ), m_alive( false ), m_fd( -1 ) {}

bool ReadableFifoPipe::open( const string &name, std::error_code &err ) {
	if( m_fd >= 0 ) {
		err = std::make_error_code( std::errc::already_connected );
		return false;
	}

	m_filePath = (BaseDir::temp() / name).u8string();
	if( mkfifo( m_filePath.c_str(), 0600 ) != 0 ) {
		err = std::make_error_code( (std::errc)errno );
		m_filePath.clear();
		return false;
	}

	m_fd = ::open( m_filePath.c_str(), O_RDONLY | O_NONBLOCK | O_NOFOLLOW | O_ASYNC );
	if( m_fd < 0 ) {
		err = std::make_error_code( (std::errc)errno );
		unlink( m_filePath.c_str() );
		m_filePath.clear();
		return false;
	}

	::fcntl( m_fd, F_SETFD, O_NONBLOCK | O_ASYNC );

	m_alive = true;
	return true;
}

bool ReadableFifoPipe::connect( std::error_code &err ) const noexcept {
	ubyte ack = 0;
	while( m_alive ) {
		ssize_t r = ::read( m_fd, &ack, 1 );

		if( r == 1 ) {
			if( ack == 6 ) return true;

			err = std::make_error_code( std::errc::bad_message );
			return false;
		}

		if( r < 0 && errno != EAGAIN ) {
			err = std::make_error_code( m_alive ? (std::errc)errno : std::errc::operation_canceled );
			return false;
		}

		usleep( 10000 );
	}

	err = std::make_error_code( std::errc::operation_canceled );
	return false;
}

bool ReadableFifoPipe::close( std::error_code &err ) noexcept {
	if( m_fd < 0 ) return true;

	m_alive = false;
	if( ::close( m_fd ) != 0 ) {
		err = std::make_error_code( (std::errc)errno );
		::unlink( m_filePath.c_str() );
		return false;
	}

	m_fd = -1;
	if( ::unlink( m_filePath.c_str() ) != 0 ) {
		err = std::make_error_code( (std::errc)errno );
		return false;
	}
	
	m_filePath.clear();
	return true;
}

bool ReadableFifoPipe::read( size_t bytes, void *buffer, std::error_code &err ) const noexcept {
	while( m_alive && bytes > 0 ) {
		pollfd fd { m_fd, POLLIN, 0 };
		const int status = ::poll( &fd, 1, 150 );
		if( status == 0 ) continue;

		if( status < 0 ) {
			err = std::make_error_code( m_alive ? (std::errc)errno : std::errc::operation_canceled );
			return false;
		}

		const ssize_t bytesRead = ::read( m_fd, buffer, bytes );
		if( bytesRead < 0 ) {
			err = std::make_error_code( m_alive ? (std::errc)errno : std::errc::operation_canceled );
			return false;
		} else if( bytesRead == 0 ) {
			err = std::make_error_code( m_alive ? std::errc::no_message : std::errc::operation_canceled );
			return false;
		}

		buffer = ((char*)buffer) + bytesRead;
		bytes -= bytesRead;
	}

	if( bytes ) {
		err = std::make_error_code( std::errc::operation_canceled );
		return false;
	}

	return true;
}

/* Output */

WritableFifoPipe::WritableFifoPipe() noexcept : m_filePath( "" ), m_cancelled( false ), m_fd( -1 ), m_created( false ) {}

bool WritableFifoPipe::open( const string &name, std::error_code &err ) {
	if( m_created ) {
		err = std::make_error_code( std::errc::already_connected );
		return false;
	}

	m_filePath = (BaseDir::temp() / name).u8string();
	if( mkfifo( m_filePath.c_str(), 0600 ) != 0 ) {
		err = std::make_error_code( (std::errc)errno );
		m_filePath.clear();
		return false;
	}

	// Don't actually open it yet. Do that in connect() instead
	return true;
}

bool WritableFifoPipe::connect( std::error_code &err ) noexcept {
	while( !m_cancelled ) {
		m_fd = ::open( m_filePath.c_str(), O_WRONLY | O_NONBLOCK | O_NOFOLLOW | O_ASYNC );
		if( m_fd > 0 ) break;

		if( errno == ENXIO ) {
			usleep( 10000 );
			continue;
		}

		err = std::make_error_code( (std::errc)errno );
		return false;
	}

	if( m_cancelled ) {
		err = std::make_error_code( std::errc::operation_canceled );
		return false;
	}

	if( ::fcntl( m_fd, F_SETFD, O_ASYNC ) && ::fcntl( m_fd, F_SETFD, 0 ) ) {
		err = std::make_error_code( (std::errc)errno );
		return false;
	}

	const ubyte ack = 6;
	return write( 1, &ack, err );
}

bool WritableFifoPipe::close( std::error_code &err ) noexcept {
	if( m_fd < 0 ) return true;

	m_cancelled = true;
	if( ::close( m_fd ) != 0 ) {
		err = std::make_error_code( (std::errc)errno );
		::unlink( m_filePath.c_str() );
		return false;
	}

	m_fd = -1;
	if( ::unlink( m_filePath.c_str() ) != 0 ) {
		err = std::make_error_code( (std::errc)errno );
		return false;
	}
	
	m_created = false;
	m_filePath.clear();
	return true;
}

bool WritableFifoPipe::write( size_t bytes, const void *buffer, std::error_code &err ) const noexcept {
	while( bytes > 0 && !m_cancelled ) {
		ssize_t bytesWritten = ::write( m_fd, buffer, bytes );
		if( bytesWritten < 0 ) {
			err = m_cancelled ? std::make_error_code( std::errc::operation_canceled ) : std::make_error_code( (std::errc)errno );
			return false;
		}

		bytes -= bytesWritten;
		buffer = (const char*)buffer + bytesWritten;
	}

	if( bytes ) {
		err = std::make_error_code( std::errc::operation_canceled );
		return false;
	}

	return true;
}

#endif
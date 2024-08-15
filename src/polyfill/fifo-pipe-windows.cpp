#ifdef _WIN32

#include "src/polyfill/fifo-pipe.hpp"

#include <cstring>
#include <thread>
#include <windows.h>
#include <winerror.h>

/* Input */

ReadableFifoPipe::ReadableFifoPipe() noexcept : m_filePath( "" ), m_alive( false ), m_handle( nullptr ) {}

static inline bool waitForConnection( const bool &alive, void *handle, std::error_code &err ) noexcept {
	OVERLAPPED status;
	std::memset( &status, 0, sizeof(OVERLAPPED) );

	if( ConnectNamedPipe( handle, &status ) ) return true;
	DWORD statusCode = GetLastError();

	if( statusCode == ERROR_PIPE_CONNECTED ) return true;

	if( statusCode != ERROR_IO_PENDING && statusCode != ERROR_PIPE_LISTENING ) {
		err = alive ? std::error_code( (int)statusCode, std::system_category() ) : std::make_error_code( std::errc::operation_canceled );
		CancelIo( handle );
		return false;
	}

	while( alive ) {
		DWORD scratch;
		if( GetOverlappedResultEx( handle, &status, &scratch, 150, false ) ) {
			return true;
		}

		statusCode = GetLastError();
		if( statusCode != WAIT_TIMEOUT ) {
			err = alive ? std::error_code( (int)statusCode, std::system_category() ) : std::make_error_code( std::errc::operation_canceled );
			CancelIo( handle );
			return false;
		}
	}

	CancelIo( handle );
	err = std::make_error_code( std::errc::operation_canceled );
	return false;
}

bool ReadableFifoPipe::open( const string &name, std::error_code &err ) {
	if( m_handle ) {
		err = std::make_error_code( std::errc::already_connected );
		return false;
	}

	m_filePath = "\\\\.\\pipe\\"s + name;
	m_handle = CreateNamedPipeA(
		m_filePath.c_str(),
		PIPE_ACCESS_INBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
		PIPE_READMODE_BYTE,
		2,
		4096,
		4096,
		0,
		nullptr
	);

	if( m_handle == INVALID_HANDLE_VALUE ) {
		err = std::error_code( (int)GetLastError(), std::system_category() );
		m_filePath.clear();
		m_handle = nullptr;
		return false;
	}

	m_alive = true;
	return true;
}

bool ReadableFifoPipe::connect( std::error_code &err ) const noexcept {
	ubyte ack = 0;
	if( !waitForConnection( m_alive, m_handle, err ) ) return false;
	if( !read( 1, &ack, err ) ) return false;

	if( ack != 6 ) {
		err = std::make_error_code( std::errc::bad_message );
		return false;
	}

	return true;
}

bool ReadableFifoPipe::close( std::error_code &err ) noexcept {
	if( !m_handle ) return true;
	
	m_alive = false;
	if( !CloseHandle( m_handle ) ) {
		err = std::error_code( (int)GetLastError(), std::system_category() );
		return false;
	}

	m_handle = nullptr;
	m_filePath.clear();
	return true;
}

bool ReadableFifoPipe::read( size_t bytes, void *buffer, std::error_code &err ) const noexcept {
	if( bytes == 0 ) return true;

	OVERLAPPED status;
	std::memset( &status, 0, sizeof(OVERLAPPED) );

	if(
		!ReadFile( m_handle, buffer, (DWORD)bytes, nullptr, &status ) &&
		GetLastError() != ERROR_IO_PENDING
	) return false;

	DWORD bytesRead = 0;
	while( m_alive ) {
		if( GetOverlappedResultEx( m_handle, &status, &bytesRead, 150, false ) ) {
			if( bytesRead != bytes ) {
				CancelIo( m_handle );
				err = m_alive ? std::make_error_code( std::errc::message_size ) : std::make_error_code( std::errc::operation_canceled );
				return false;
			}

			return true;
		}

		const DWORD errorCode = GetLastError();
		if( errorCode != WAIT_TIMEOUT ) {
			CancelIo( m_handle );
			err = m_alive ? std::error_code( (int)errorCode, std::system_category() ) : std::make_error_code( std::errc::operation_canceled );
			return false;
		}
	}

	CancelIo( m_handle );
	err = std::make_error_code( std::errc::operation_canceled );
	return false;
}

/* Output */

WritableFifoPipe::WritableFifoPipe() noexcept : m_filePath( "" ), m_alive( false ), m_handle( nullptr ) {}

bool WritableFifoPipe::open( const string &name, std::error_code &err ) {
	if( m_handle ) {
		err = std::make_error_code( std::errc::already_connected );
		return false;
	}

	m_filePath = "\\\\.\\pipe\\"s + name;
	m_handle = CreateNamedPipeA(
		m_filePath.c_str(),
		PIPE_ACCESS_OUTBOUND | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE,
		2,
		4096,
		4096,
		0,
		nullptr
	);

	if( m_handle == INVALID_HANDLE_VALUE ) {
		err = std::error_code( (int)GetLastError(), std::system_category() );
		m_filePath.clear();
		m_handle = nullptr;
		return false;
	}

	m_alive = true;
	return true;
}

bool WritableFifoPipe::connect( std::error_code &err ) noexcept {
	if( !waitForConnection( m_alive, m_handle, err ) ) return false;

	const byte ack = 6;
	return write( 1, &ack, err );
}

bool WritableFifoPipe::close( std::error_code &err ) noexcept {
	if( !m_handle ) return true;
	
	m_alive = false;
	if( !CloseHandle( m_handle ) ) {
		err = std::error_code( (int)GetLastError(), std::system_category() );
		return false;
	}

	m_handle = nullptr;
	m_filePath.clear();
	return true;
}

bool WritableFifoPipe::write( size_t bytes, const void *buffer, std::error_code &err ) const noexcept {
	if( bytes == 0 ) return true;

	OVERLAPPED status;
	std::memset( &status, 0, sizeof(OVERLAPPED) );

	if( !WriteFile( m_handle, buffer, (DWORD)bytes, nullptr, &status ) ) {
		err = std::error_code( (int)GetLastError(), std::system_category() );
		return false;
	}

	DWORD bytesWritten = 0;
	while( m_alive ) {
		if( GetOverlappedResultEx( m_handle, &status, &bytesWritten, 150, false ) ) {
			if( !FlushFileBuffers( m_handle ) ) {
				err = std::error_code( (int)GetLastError(), std::system_category() );
				return false;
			}

			if( bytesWritten < bytes ) {
				buffer = (const void*)((const char*)buffer + bytesWritten);
				bytes -= bytesWritten;

				std::this_thread::yield();
				WriteFile( m_handle, buffer, (DWORD)bytes, nullptr, &status );
				continue;
			}

			return true;
		}

		const DWORD errorCode = GetLastError();
		if( errorCode != WAIT_TIMEOUT ) {
			CancelIo( m_handle );
			err = m_alive ? std::error_code( (int)errorCode, std::system_category() ) : std::make_error_code( std::errc::operation_canceled );
			return false;
		}
	}

	CancelIo( m_handle );
	err = std::make_error_code( std::errc::operation_canceled );
	return false;
}

#endif
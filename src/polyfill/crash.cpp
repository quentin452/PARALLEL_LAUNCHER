#include "src/polyfill/crash.hpp"

#include "src/core/traceable-exception.hpp"
#include "src/core/app-context.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/logging.hpp"
#include "src/core/time.hpp"
#include "src/polyfill/backtrace.hpp"
#include "src/polyfill/file.hpp"
#include "src/db/data-provider.hpp"
#include <exception>
#include <typeinfo>
#include <cstdlib>

#ifdef _WIN32

#include <Windows.h>
#include <cstdio>
#include "src/core/numeric-string.hpp"
#include "src/types.hpp"

static void onTerminate();

static std::FILE *startCrashLog( const char *errorType, const char *errorMessage, bool isWarning ) noexcept {
	const AppContextInfo &appContext = getAppContext();
	std::FILE *file = fopen( appContext.crashlogFilepath, "wb" );
	if( !file ) return nullptr;

	FILETIME ftime;
	GetSystemTimeAsFileTime( &ftime );

	ULARGE_INTEGER bigint;
	bigint.u.LowPart = ftime.dwLowDateTime;
	bigint.u.HighPart = ftime.dwHighDateTime;

	int64 now = reinterpret_cast<int64&>( bigint.QuadPart );
	now /= 10000000ll;
	now -= 11644473600l;

	fprintf( file, "[Context]\n" );
	fprintf( file, "StartedAt = %lld\n", appContext.appStartTime );
	fprintf( file, "CrashedAt = %lld\n", now );

	if( appContext.deviceId ) {
		fprintf( file, "DeviceId = %s\n", appContext.deviceId );
		fprintf( file, "AppVersion = %s\n", appContext.appVersion );
		fprintf( file, "Screen = %dx%d@%ddpi\n", appContext.screenWidth, appContext.screenHeight, appContext.screenDpiY );
		fprintf( file, "Locale = %s\n", appContext.locale );
	}

	if( isWarning ) {
		fprintf( file, "\n[Warning]\n" );
	} else {
		fprintf( file, "\n[Error]\n" );
	}

	fprintf( file, "Type = %s\n", errorType );
	if( errorMessage ) fprintf( file, "Message = %s\n", errorMessage );

	fprintf( file, "\n[Backtrace]\n" );
	return file;
}

static inline void endCrashLog( std::FILE *log ) noexcept {
	if( !log ) return;
	std::fflush( log );
	std::fclose( log );
}

static LONG WINAPI onWindowsException( LPEXCEPTION_POINTERS seh ) {
	const std::exception_ptr ep = std::current_exception();
	if( ep ) {
		try {
			std::rethrow_exception( ep );
		} catch( const std::exception &exception ) {
			const auto out = startCrashLog( typeid( exception ).name(), exception.what(), false );
			const TraceableException *traceableException = dynamic_cast<const TraceableException*>( &exception );
			if( traceableException != nullptr ) {
				traceableException->backtrace().log( out );
			} else {
				Backtrace::logFromContext( out, (void*)seh->ContextRecord );
			}
			endCrashLog( out );
		} catch( ... ) {
			const auto out = startCrashLog( "Unhandled Exception", nullptr, false );
			Backtrace::logFromContext( out, (void*)seh->ContextRecord );
			endCrashLog( out );
		}
	} else {
		PEXCEPTION_RECORD exception = seh->ExceptionRecord;
		if( exception == nullptr ) {
			const auto out = startCrashLog( "Structured Exception Event", nullptr, false );
			Backtrace::logFromContext( out, (void*)seh->ContextRecord );
			endCrashLog( out );
		} else {
			while( exception->ExceptionRecord != nullptr ) {
				exception = exception->ExceptionRecord;
			}

			const auto out = startCrashLog( "Structured Exception Event", Number::toString( (uint)exception->ExceptionCode ).c_str(), false );
			Backtrace::logFromContext( out, (void*)seh->ContextRecord );
			endCrashLog( out );
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}


void registerCrashHandlers() {
	SetUnhandledExceptionFilter( onWindowsException );
	std::set_terminate( onTerminate );
}

#else

#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

static void onTerminate();

static int startCrashLog( const char *errorType, const char *errorMessage, bool isWarning ) noexcept {
	const AppContextInfo &appContext = getAppContext();
	int fd = open( appContext.crashlogFilepath, O_WRONLY | O_CREAT | O_TRUNC, 0644 );
	if( fd < 0 ) return fd;

	dprintf( fd, "[Context]\n" );
	dprintf( fd, "StartedAt = %lld\n", appContext.appStartTime );
	dprintf( fd, "CrashedAt = %lld\n", (int64)time( nullptr ) );

	if( appContext.deviceId ) {
		dprintf( fd, "DeviceId = %s\n", appContext.deviceId );
		dprintf( fd, "AppVersion = %s\n", appContext.appVersion );
		dprintf( fd, "Screen = %dx%d@%ddpi\n", appContext.screenWidth, appContext.screenHeight, appContext.screenDpiY );
		dprintf( fd, "Locale = %s\n", appContext.locale );
	}

	if( isWarning ) {
		dprintf( fd, "\n[Warning]\n" );
	} else {
		dprintf( fd, "\n[Error]\n" );
	}

	dprintf( fd, "Type = %s\n", errorType );
	if( errorMessage ) dprintf( fd, "Message = %s\n", errorMessage );

	dprintf( fd, "\n[Backtrace]\n" );
	return fd;
}

static inline void endCrashLog( int log ) noexcept {
	if( log < 0 ) return;
	fsync( log );
	close( log );
}

extern "C" void onSegfault( int ) {
	const int fd = startCrashLog( "Segmentation Fault", nullptr, false );
	Backtrace().log( fd );
	endCrashLog( fd );
	std::abort();
}

void registerCrashHandlers() {
	std::signal( SIGSEGV, onSegfault );
	std::set_terminate( onTerminate );
}

#endif

static void onTerminate() {
	const std::exception_ptr ep = std::current_exception();
	if( !ep ) return;

	try {
		std::rethrow_exception( ep );
	} catch( const std::exception &exception ) {
		const auto out = startCrashLog( typeid( exception ).name(), exception.what(), false );
		const TraceableException *traceableException = dynamic_cast<const TraceableException*>( &exception );
		if( traceableException != nullptr ) {
			traceableException->backtrace().log( out );
		} else {
			Backtrace().log( out );
		}
		endCrashLog( out );
	} catch( ... ) {
#ifdef __linux__
		const auto exceptionType = ep.__cxa_exception_type();
		const auto out = startCrashLog( "Unhandled Exception", exceptionType ? exceptionType->name() : nullptr, false );
#else
		const auto out = startCrashLog( "Unhandled Exception", nullptr, false );
#endif
		Backtrace().log( out );
		endCrashLog( out );
	}

	std::abort();
}

void __log_error_internal( const char *type, const char *message, bool isWarning, const Backtrace &backtrace ) {
	const auto out = startCrashLog( type, message, isWarning );
	backtrace.log( out );
	endCrashLog( out );
}

void storeCrashLog() {
	fs::error_code err;
	const fs::path crashlogPath = fs::to_path( getAppContext().crashlogFilepath );
	if( !fs::exists( crashlogPath, err ) || err ) return;

#ifdef __APPLE__
	const int64 timestamp = (int64)fs::last_write_time( crashlogPath, err );
#else
	const int64 timestamp = Time::toUnixTime( fs::last_write_time( crashlogPath, err ) );
#endif
	
	if( err ) return;

	string report;
	report.reserve( 4096 );

	try {
		InputFile crashlog( crashlogPath, true );
		if( crashlog.bad() ) return;

		while( crashlog.good() && !crashlog.eof() ) {
			char buffer[4096];
			crashlog.read( buffer, 4095 );
			buffer[crashlog.gcount()] = '\0';
			report += buffer;
		}

		if( crashlog.bad() && !crashlog.eof() ) {
			return;
		}

		if( !report.empty() ) {
			DataProvider::addCrashLog( timestamp, report );

			#ifdef DEBUG
			// Don't send reports from local dev build
			DataProvider::setCrashLogSentDate( timestamp, Time::now() );
			#endif
		}

	} catch( const std::exception &exception ) {
		logError( "Failed to read crashlog: "s + exception.what() );
	}

	fs::remove( crashlogPath, err );
}

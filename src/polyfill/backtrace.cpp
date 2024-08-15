#include "src/polyfill/backtrace.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <dbghelp.h>
#include <cstdio>

Backtrace::Backtrace() noexcept {
	ULONG hash;
	m_size = (int)CaptureStackBackTrace( 1, 32, m_trace, &hash );
}

static void logStackFrame( std::FILE *logFile, DWORD64 frameAddr, HANDLE process, SYMBOL_INFO *symbol, IMAGEHLP_MODULE64 *moduleInfo ) noexcept {
	if( SymFromAddr( process, frameAddr, 0, symbol ) ) {
		const char *modulePath = nullptr;
		if( symbol->ModBase ) {
			if( moduleInfo->BaseOfImage == symbol->ModBase ) {
				if( moduleInfo->ImageName[0] ) {
					modulePath = moduleInfo->ImageName;
				}
			} else if( SymGetModuleInfo64( process, symbol->ModBase, moduleInfo ) ) {
				if( moduleInfo->ImageName[0] ) {
					moduleInfo->ImageName[255] = '\0';
					modulePath = moduleInfo->ImageName;
				}
			} else {
				moduleInfo->BaseOfImage = 0;
				moduleInfo->ImageName[0] = '\0';
			}
		}

		if( modulePath ) {
			std::fprintf( logFile, "%s (0x%0llX) [%s]\n", symbol->Name, symbol->Address, modulePath );
		} else {
			std::fprintf( logFile, "%s (0x%0llX)\n", symbol->Name, symbol->Address );
		}
	} else {
		std::fprintf( logFile, "??? (0x%0llX)\n", frameAddr );
	}
}

void Backtrace::log( std::FILE *logFile ) const noexcept {
	if( !logFile || m_size < 1 ) return;
	
	HANDLE proc = GetCurrentProcess();
	SymInitialize( proc, nullptr, TRUE );

	unsigned char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
	SYMBOL_INFO *symbol = (SYMBOL_INFO*)(void*)symbolBuffer;

	symbol->MaxNameLen = MAX_SYM_NAME - 1;
	symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

	IMAGEHLP_MODULE64 moduleInfo;
	moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
	moduleInfo.BaseOfImage = 0;
	moduleInfo.ImageName[0] = '\0';

	for( int i = 0; i < m_size; i++ ) {
		logStackFrame( logFile, (DWORD64)m_trace[i], proc, symbol, &moduleInfo );
	}
}

void Backtrace::logFromContext( std::FILE *logFile, void *context ) noexcept {
	if( !logFile || !context ) return;
	
	HANDLE proc = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();
	SymInitialize( proc, nullptr, TRUE );

	unsigned char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
	SYMBOL_INFO *symbol = (SYMBOL_INFO*)(void*)symbolBuffer;

	symbol->MaxNameLen = MAX_SYM_NAME - 1;
	symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

	PCONTEXT cpuContext = (PCONTEXT)context;
	STACKFRAME64 frame;

	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Mode = AddrModeFlat;

#ifdef _WIN64
	constexpr DWORD cpuArch = IMAGE_FILE_MACHINE_AMD64;
	frame.AddrPC.Offset = cpuContext->Rip;
	frame.AddrFrame.Offset = cpuContext->Rbp;
	frame.AddrStack.Offset = cpuContext->Rsp;
#else
	constexpr DWORD cpuArch = IMAGE_FILE_MACHINE_I386;
	frame.AddrPC.Offset = cpuContext->Eip;
	frame.AddrFrame.Offset = cpuContext->Ebp;
	frame.AddrStack.Offset = cpuContext->Esp;
#endif

	IMAGEHLP_MODULE64 moduleInfo;
	moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
	moduleInfo.BaseOfImage = 0;
	moduleInfo.ImageName[0] = '\0';

	while( StackWalk64(
		cpuArch,
		proc,
		thread,
		&frame,
		cpuContext,
		nullptr,
		SymFunctionTableAccess64,
		SymGetModuleBase64,
		nullptr
	)) {
		logStackFrame( logFile, frame.AddrPC.Offset, proc, symbol, &moduleInfo );
	}
}

#else
#include <execinfo.h>

Backtrace::Backtrace() noexcept {
	m_size = ::backtrace( m_trace, 32 );
}

void Backtrace::log( int fd ) const noexcept {
	if( fd < 0 || m_size < 2 ) return;
	backtrace_symbols_fd( &m_trace[1], m_size - 1, fd );
}

#endif

#include "src/polyfill/env.hpp"

#ifdef _WIN32

#include <windows.h>
#include "src/polyfill/windows/unicode.hpp"

bool Environment::set( const char *name, const char *value ) {
	const std::wstring name16 = Unicode::toUtf16( name );
	const std::wstring value16 = Unicode::toUtf16( value );
	return SetEnvironmentVariable( name16.c_str(), value16.c_str() );
}

bool Environment::clear( const char *name ) {
	const std::wstring name16 = Unicode::toUtf16( name );
	return SetEnvironmentVariable( name16.c_str(), nullptr );
}

#else

#include <stdlib.h>

bool Environment::set( const char *name, const char *value ) {
	return setenv( name, value, 1 ) == 0;
}

bool Environment::clear( const char *name ) {
	return unsetenv( name ) == 0;
}

#endif

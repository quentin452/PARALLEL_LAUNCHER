#include "src/polyfill/windows/locale.hpp"

#ifdef _WIN64
#define _AMD64_
#else
#define _X86_
#endif

#include <winnls.h>
#include <QLocale>
#include "src/polyfill/windows/unicode.hpp"

std::string WindowsLocale::getUserLocale() {
	wchar_t buffer[LOCALE_NAME_MAX_LENGTH];
	if(
		GetUserDefaultLocaleName( buffer, LOCALE_NAME_MAX_LENGTH ) > 0 ||
		GetSystemDefaultLocaleName( buffer, LOCALE_NAME_MAX_LENGTH ) > 0
	) return Unicode::toUtf8( buffer );
	return QLocale::system().name().toStdString();
}

#ifndef SRC_POLYFILL_WINDOWS_CODEPAGE_HPP_
#define SRC_POLYFILL_WINDOWS_CODEPAGE_HPP_

#include <string>
#include <vector>

namespace Unicode {

	extern std::string latinToUtf8( const std::string &lstr );

	extern std::wstring toUtf16( const char *u8str );
	extern std::string toUtf8( const wchar_t *u16str );

	inline std::wstring toUtf16( const std::string &u8str ) {
		return toUtf16( u8str.c_str() );
	}

    std::string toUtf8(const wchar_t* wstr) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    }

	extern std::vector<std::string> argvUtf8();

}

#endif /* SRC_POLYFILL_WINDOWS_CODEPAGE_HPP_ */

#ifndef SRC_POLYFILL_WINDOWS_CODEPAGE_HPP_
#define SRC_POLYFILL_WINDOWS_CODEPAGE_HPP_

#include <string>
#include <vector>

namespace Unicode {

extern std::string latinToUtf8(const std::string &lstr);

extern std::wstring toUtf16(const char *u8str);
extern std::string toUtf8(const wchar_t *u16str);

inline std::wstring toUtf16(const std::string &u8str) {
    return toUtf16(u8str.c_str());
}

inline std::string toUtf8(const std::wstring &u16str) {
    return toUtf8(u16str.c_str());
}

extern std::vector<std::string> argvUtf8();

} // namespace Unicode

#endif /* SRC_POLYFILL_WINDOWS_CODEPAGE_HPP_ */

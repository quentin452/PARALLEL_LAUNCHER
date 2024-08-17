#include "src/polyfill/windows/unicode.hpp"

#include "src/core/buffer.hpp"
#include "src/core/utf8.hpp"
#include <codecvt>
#include <exception>
#include <locale>
#include <windows.h>

std::string Unicode::latinToUtf8(const std::string &lstr) {
  Buffer wideBuffer(lstr.size() * 2 + 2);
  if (MultiByteToWideChar(CP_ACP, 0, lstr.data(), (int)lstr.size() + 1,
                          (wchar_t *)wideBuffer.data(),
                          (int)lstr.size() * 2 + 2)) {
    return Unicode::toUtf8((const wchar_t *)wideBuffer.data());
  }

  try {
    Unicode::toUtf16(lstr);
    return lstr;
  } catch (const std::exception &) {
    std::string safeString;
    safeString.reserve(lstr.length());
    for (char c : lstr) {
      if (((unsigned char)c) & 0x80)
        continue;
      safeString += c;
    }
    return safeString;
  }
}

std::wstring Unicode::toUtf16(const char *u8str) {
  try {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.from_bytes(u8str);
  } catch (const std::exception &) {
    std::string safeString;
    Utf8::validate(u8str, &safeString);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.from_bytes(safeString);
  }
}

std::string Unicode::toUtf8(const wchar_t *u16str) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
  return converter.to_bytes(u16str);
}

std::vector<std::string> Unicode::argvUtf8() {
  int argc;
  wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  std::vector<std::string> args;
  if (argv == nullptr)
    return args;

  args.reserve(argc);
  for (int i = 0; i < argc; i++) {
    args.push_back(Unicode::toUtf8(argv[i]));
  }

  LocalFree(argv);
  return args;
}

#include "src/thirdparty/miniz_patch.h"

#ifdef _WIN32
#include <Windows.h>

void mz_utf8_to_utf16(const char *src, wchar_t *dest, int maxChars) {
  MultiByteToWideChar(
      /* CodePage */ CP_UTF8,
      /* dwFlags */ 0,
      /* lpMultiByteStr */ src,
      /* cbMultiByte */ -1,
      /* lpWideCharStr */ dest,
      /* cchWideChar */ maxChars);
  dest[maxChars - 1] = '\0';
}

#endif

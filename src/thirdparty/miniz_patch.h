#ifndef SRC_THIRDPARTY_MINIZ_PATCH_H_
#define SRC_THIRDPARTY_MINIZ_PATCH_H_

#ifdef _WIN32

#include <wchar.h>

extern void mz_utf8_to_utf16( const char *src, wchar_t* dest, int maxChars );

#endif

#endif /* SRC_THIRDPARTY_MINIZ_PATCH_H_ */

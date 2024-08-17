#include "src/core/utf8.hpp"

#include <cstdint>

bool Utf8::validate(const std::string &str, std::string *safeString) {
  if (safeString) {
    safeString->clear();
    safeString->reserve(str.length());
  }

  char buffer[4];
  bool valid = true;
  int_fast8_t cpl = 1;
  int_fast8_t i = 0;

  for (char c : str) {
    if (!valid && !safeString)
      return false;

    unsigned char b = (unsigned char)c;
    if (b & 0x80) {
      buffer[i] = c;
      if (i++ == 0) {
        if (b >> 3 == 0x1e) {
          cpl = 4;
        } else if (b >> 4 == 0xe) {
          cpl = 3;
        } else if (b >> 5 == 0x6) {
          cpl = 2;
        } else {
          valid = false;
          i = 0;
        }
      } else if (b >> 6 != 0x2) {
        valid = false;
        i = 0;
      } else if (i == cpl) {
        if (cpl == 4) {
          if ((((unsigned char)buffer[0] & 0xf) > 4) ||
              (((unsigned char)buffer[0] & 0xf) &&
               ((unsigned char)buffer[1] & 0x30))) {
            valid = false;
            i = 0;
            continue;
          }
        }

        if (safeString) {
          for (i = 0; i < cpl; i++)
            safeString->push_back(buffer[i]);
        }
        i = 0;
      }
    } else if (i == 0) {
      if (safeString)
        safeString->push_back(c);
    } else {
      valid = false;
      i = 0;
    }
  }

  return valid && !i;
}

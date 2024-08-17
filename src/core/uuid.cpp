#include "src/core/uuid.hpp"

#include "src/polyfill/random.hpp"
#include <cstring>

Uuid::Uuid() noexcept { std::memset(m_data, 0, 16); }

Uuid::Uuid(const Uuid &other) noexcept {
  std::memcpy(m_data, other.m_data, 16);
}

Uuid::Uuid(const ubyte *data) noexcept { std::memcpy(m_data, data, 16); }

Uuid &Uuid::operator=(const Uuid &other) noexcept {
  std::memcpy(m_data, other.m_data, 16);
  return *this;
}

static constexpr char NUMERAL_OFFSET = '0';
static constexpr char LOWER_ALPHA_OFFSET = 'a' - (char)10;
static constexpr char UPPER_ALPHA_OFFSET = 'A' - (char)10;

static void writeHex(char *out, const ubyte *data, size_t numBytes) noexcept {
  for (size_t i = 0; i < numBytes; i++) {
    const ubyte upperNibble = data[i] >> 4;
    const ubyte lowerNibble = data[i] & 0xF;
    out[i * 2] =
        upperNibble + (upperNibble > 9 ? LOWER_ALPHA_OFFSET : NUMERAL_OFFSET);
    out[i * 2 + 1] =
        lowerNibble + (lowerNibble > 9 ? LOWER_ALPHA_OFFSET : NUMERAL_OFFSET);
  }
}

static inline bool tryReadNibble(char c, ubyte &out) {
  if (c < '0') {
    return false;
  } else if (c <= '9') {
    out = c - NUMERAL_OFFSET;
    return true;
  } else if (c < 'A') {
    return false;
  } else if (c <= 'Z') {
    out = c - UPPER_ALPHA_OFFSET;
    return true;
  } else if (c < 'a') {
    return false;
  } else if (c <= 'z') {
    out = c - LOWER_ALPHA_OFFSET;
    return true;
  } else
    return false;
}

static bool tryReadHex(ubyte *out, const char *str, size_t numBytes) noexcept {
  for (size_t i = 0; i < numBytes; i++) {
    ubyte upperNibble, lowerNibble;
    if (tryReadNibble(str[i * 2], upperNibble) &&
        tryReadNibble(str[i * 2 + 1], lowerNibble)) {
      out[i] = (upperNibble << 4) | lowerNibble;
    } else
      return false;
  }
  return true;
}

string Uuid::toString() const noexcept {
  char str[37] = "00000000-0000-0000-0000-000000000000";
  writeHex(str, m_data, 4);
  writeHex(&str[9], &m_data[4], 2);
  writeHex(&str[14], &m_data[6], 2);
  writeHex(&str[19], &m_data[8], 2);
  writeHex(&str[24], &m_data[10], 6);
  return string(str);
}

bool Uuid::tryParse(const string &str, Uuid &out) noexcept {
  const char *const cstr = str.c_str();
  return (str.length() == 36 && cstr[8] == '-' && cstr[13] == '-' &&
          cstr[18] == '-' && cstr[23] == '-' &&
          tryReadHex(out.m_data, cstr, 4) &&
          tryReadHex(&out.m_data[4], &cstr[9], 2) &&
          tryReadHex(&out.m_data[6], &cstr[14], 2) &&
          tryReadHex(&out.m_data[8], &cstr[19], 2) &&
          tryReadHex(&out.m_data[10], &cstr[24], 6));
}

int Uuid::compare(const Uuid &lhs, const Uuid &rhs) noexcept {
  return std::memcmp(lhs.m_data, rhs.m_data, 16);
}

Uuid Uuid::Random() {
  Uuid rand;
  RNG::getBytes(rand.m_data, 16);
  rand.m_data[6] = (rand.m_data[6] & 0x0F) | 0x40;
  rand.m_data[8] = (rand.m_data[8] & 0x3F) | 0x80;
  return rand;
}

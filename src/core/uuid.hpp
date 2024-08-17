#ifndef SRC_CORE_UUID_HPP_
#define SRC_CORE_UUID_HPP_

#include "src/core/traceable-exception.hpp"
#include "src/types.hpp"
#include <cstdint>
#include <functional>
#include <stdexcept>

class UuidParseException : public std::runtime_error, TraceableException {

private:
  const string m_badUuid;

public:
  UuidParseException(const string &badUuid)
      : std::runtime_error("Failed to parse string as UUID: " + badUuid),
        TraceableException(), m_badUuid(badUuid) {}

  inline const string &badUuid() const noexcept { return m_badUuid; }
};

class alignas(uint64_t) Uuid final {

private:
  ubyte m_data[16];

public:
  Uuid() noexcept;
  Uuid(const Uuid &other) noexcept;
  Uuid(const ubyte *data) noexcept;

  Uuid &operator=(const Uuid &other) noexcept;

  inline const ubyte *data() const noexcept { return m_data; }

  string toString() const noexcept;

  static bool tryParse(const string &str, Uuid &out) noexcept;
  inline static Uuid parse(const string &str) {
    Uuid uuid;
    if (!tryParse(str, uuid)) {
      throw UuidParseException(str);
    }
    return uuid;
  }

  static int compare(const Uuid &lhs, const Uuid &rhs) noexcept;
  inline static const Uuid &empty() {
    static const Uuid s_zero;
    return s_zero;
  }

  static Uuid Random();
};

template <> struct std::hash<Uuid> {
  inline size_t operator()(const Uuid &x) const {
    static_assert(sizeof(size_t) == 8 || sizeof(size_t) == 4);
    const size_t *data = (const size_t *)&x;
    if constexpr (sizeof(size_t) == 8) {
      return data[0] ^ data[1];
    } else {
      return (data[0] ^ data[1]) ^ (data[2] ^ data[3]);
    }
  }
};

inline bool operator==(const Uuid &lhs, const Uuid &rhs) noexcept {
  return Uuid::compare(lhs, rhs) == 0;
}
inline bool operator!=(const Uuid &lhs, const Uuid &rhs) noexcept {
  return Uuid::compare(lhs, rhs) != 0;
}
inline bool operator<(const Uuid &lhs, const Uuid &rhs) noexcept {
  return Uuid::compare(lhs, rhs) < 0;
}
inline bool operator<=(const Uuid &lhs, const Uuid &rhs) noexcept {
  return Uuid::compare(lhs, rhs) <= 0;
}
inline bool operator>(const Uuid &lhs, const Uuid &rhs) noexcept {
  return Uuid::compare(lhs, rhs) > 0;
}
inline bool operator>=(const Uuid &lhs, const Uuid &rhs) noexcept {
  return Uuid::compare(lhs, rhs) >= 0;
}

#endif /* SRC_CORE_UUID_HPP_ */

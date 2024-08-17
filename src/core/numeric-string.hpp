#ifndef SRC_CORE_NUMERIC_STRING_HPP_
#define SRC_CORE_NUMERIC_STRING_HPP_

#include "src/types.hpp"

namespace Number {

extern string toString(sbyte x) noexcept;
extern string toString(short x) noexcept;
extern string toString(int x) noexcept;
extern string toString(long x) noexcept;
extern string toString(int64 x) noexcept;

extern string toString(ubyte x) noexcept;
extern string toString(ushort x) noexcept;
extern string toString(uint x) noexcept;
extern string toString(ulong x) noexcept;
extern string toString(uint64 x) noexcept;

extern string toString(float x) noexcept;
extern string toString(double x) noexcept;
extern string toString(long double x) noexcept;

extern long parseInt(const string &str);
extern ulong parseUInt(const string &str);
extern int64 parseInt64(const string &str);
extern uint64 parseUInt64(const string &str);
extern float parseFloat(const string &str);
extern double parseDouble(const string &str);
extern long double parseLongDouble(const string &str);

} // namespace Number

#endif /* SRC_CORE_NUMERIC_STRING_HPP_ */

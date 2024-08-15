#ifndef SRC_CORE_VERSION_HPP_
#define SRC_CORE_VERSION_HPP_

#include "src/types.hpp"
#include "src/core/json.hpp"

struct Version final {

	ushort major, minor, patch;

	string toString() const;
	static bool tryParse( const string &versionString, Version &result );

	static int64 compare( const Version &a, const Version &b ) {
		int64 A = ((int64)a.major << 32) + ((int64)a.minor << 16) + (int64)a.patch;
		int64 B = ((int64)b.major << 32) + ((int64)b.minor << 16) + (int64)b.patch;
		return A - B;
	}

};

inline bool operator==( const Version &lhs, const Version &rhs ) {
	return Version::compare( lhs, rhs ) == 0ll;
}
inline bool operator!=( const Version &lhs, const Version &rhs ) {
	return Version::compare( lhs, rhs ) != 0ll;
}
inline bool operator<( const Version &lhs, const Version &rhs ) {
	return Version::compare( lhs, rhs ) < 0ll;
}
inline bool operator<=( const Version &lhs, const Version &rhs ) {
	return Version::compare( lhs, rhs ) <= 0ll;
}
inline bool operator>( const Version &lhs, const Version &rhs ) {
	return Version::compare( lhs, rhs ) > 0ll;
}
inline bool operator>=( const Version &lhs, const Version &rhs ) {
	return Version::compare( lhs, rhs ) >= 0ll;
}

namespace CurrentVersion {
	extern const Version Application;
}

namespace JsonSerializer {
	template<> void serialize<Version>( JsonWriter &jw, const Version &obj );
	template<> Version parse<Version>( const Json &json );
}

#endif /* SRC_CORE_VERSION_HPP_ */

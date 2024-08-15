#include "src/core/version.hpp"

#include <regex>
#include <limits>
#include "src/core/numeric-string.hpp"

using namespace std;

static const regex VersionRegex(
	"(\\d{1,5}).(\\d{1,5}).(\\d{1,5})",
	regex_constants::ECMAScript | regex_constants::optimize
);

string Version::toString() const {
	return Number::toString( major ) + "." + Number::toString( minor ) + "." + Number::toString( patch );
}

bool Version::tryParse( const string &versionString, Version &result ) {
	smatch matches;
	if( !regex_search( versionString, matches, VersionRegex ) || matches.size() != 4 ) {
		return false;
	}

	const ulong major = Number::parseUInt( matches[1].str() );
	const ulong minor = Number::parseUInt( matches[2].str() );
	const ulong patch = Number::parseUInt( matches[3].str() );

	constexpr ushort MAX_VALUE = numeric_limits<ushort>::max();
	if( major > MAX_VALUE || minor > MAX_VALUE || patch > MAX_VALUE ) {
		return false;
	}

	result.major = (ushort)major;
	result.minor = (ushort)minor;
	result.patch = (ushort)patch;
	return true;
}

const Version CurrentVersion::Application = { 7, 7, 0 };

#ifdef _WIN32
const ushort APPLICATION_MAJOR_VERSION = CurrentVersion::Application.major;
const ushort APPLICATION_MINOR_VERSION = CurrentVersion::Application.minor;
#endif

template<> void JsonSerializer::serialize<Version>( JsonWriter &jw, const Version &obj ) {
	jw.writeString( obj.toString() );
}

template<> Version JsonSerializer::parse<Version>( const Json &json ) {
	Version version;
	if( Version::tryParse( json.get<string>(), version ) ) {
		return version;
	}

	throw JsonReaderException( "Failed to parse version string" );
}

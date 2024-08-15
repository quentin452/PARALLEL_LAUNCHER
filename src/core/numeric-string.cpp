#include "src/core/numeric-string.hpp"

#ifdef __APPLE__
#include <sstream>
#include <locale>

#define DEFINE_NUMBER_TO_STRING( atype, ctype ) \
	string Number::toString( atype x ) noexcept { \
		std::stringstream stream; \
		stream.imbue( std::locale::classic() ); \
		stream << (ctype)x; \
		return stream.str(); \
	}

#define DEFINE_STRING_TO_NUMBER( func, type ) \
	type Number::func( const string &str ) { \
		type value; \
		std::stringstream stream( str ); \
		stream.imbue( std::locale::classic() ); \
		stream >> value; \
		return value; \
	}

DEFINE_NUMBER_TO_STRING( sbyte, short )
DEFINE_NUMBER_TO_STRING( ubyte, ushort )
DEFINE_NUMBER_TO_STRING( short, short )
DEFINE_NUMBER_TO_STRING( ushort, ushort )
DEFINE_NUMBER_TO_STRING( int, int )
DEFINE_NUMBER_TO_STRING( uint, uint )
DEFINE_NUMBER_TO_STRING( long, long )
DEFINE_NUMBER_TO_STRING( ulong, ulong )
DEFINE_NUMBER_TO_STRING( int64, int64 )
DEFINE_NUMBER_TO_STRING( uint64, uint64 )
DEFINE_NUMBER_TO_STRING( float, float )
DEFINE_NUMBER_TO_STRING( double, double )
DEFINE_NUMBER_TO_STRING( long double, long double )

DEFINE_STRING_TO_NUMBER( parseInt, long )
DEFINE_STRING_TO_NUMBER( parseUInt, ulong )
DEFINE_STRING_TO_NUMBER( parseInt64, int64 )
DEFINE_STRING_TO_NUMBER( parseUInt64, uint64 )
DEFINE_STRING_TO_NUMBER( parseFloat, float )
DEFINE_STRING_TO_NUMBER( parseDouble, double )
DEFINE_STRING_TO_NUMBER( parseLongDouble, long double )

#else

#include <charconv>
#include <stdexcept>
#include <system_error>
#ifdef __GNUC__
#include <sstream>
#include <locale>
#endif
#include "src/polyfill/crash.hpp"

#define DEFINE_INT_TO_STRING( type, bufferSize ) \
	string Number::toString( type x ) noexcept { \
		char buffer[bufferSize]; \
		auto result = std::to_chars( buffer, buffer + bufferSize, x ); \
		pl_assert( result.ec == std::errc() ); \
		*result.ptr = '\0'; \
		return string( buffer ); \
	}

#ifdef __GNUC__
	#define DEFINE_FLOAT_TO_STRING( type, bufferSize ) \
		string Number::toString( type x ) noexcept { \
			std::stringstream stream; \
			stream.imbue( std::locale::classic() ); \
			stream << x; \
			return stream.str(); \
		}
#else
	#define DEFINE_FLOAT_TO_STRING( type, bufferSize ) DEFINE_INT_TO_STRING( type, bufferSize )
#endif

DEFINE_INT_TO_STRING( sbyte, 8 )
DEFINE_INT_TO_STRING( ubyte, 8 )
DEFINE_INT_TO_STRING( short, 8 )
DEFINE_INT_TO_STRING( ushort, 8 )
DEFINE_INT_TO_STRING( int, 12 )
DEFINE_INT_TO_STRING( uint, 12 )
DEFINE_INT_TO_STRING( long, 24 )
DEFINE_INT_TO_STRING( ulong, 24 )
DEFINE_INT_TO_STRING( int64, 24 )
DEFINE_INT_TO_STRING( uint64, 24 )
DEFINE_FLOAT_TO_STRING( float, 20 )
DEFINE_FLOAT_TO_STRING( double, 28 )
DEFINE_FLOAT_TO_STRING( long double, 48 )

#define DEFINE_STRING_TO_INT( func, type ) \
	type Number::func( const string &str ) { \
		type value; \
		const auto result = std::from_chars( str.data(), str.data() + str.size(), value ); \
		if( result.ec != std::errc() ) { \
			switch( result.ec ) { \
				case std::errc::invalid_argument: throw std::invalid_argument( std::make_error_code( result.ec ).message() ); \
				case std::errc::result_out_of_range: throw std::out_of_range( std::make_error_code( result.ec ).message() ); \
				default: throw std::logic_error( std::make_error_code( result.ec ).message() ); \
			} \
		} \
		return value; \
	}

#ifdef __GNUC__
	#define DEFINE_STRING_TO_FLOAT( func, type ) \
		type Number::func( const string &str ) { \
			type value; \
			std::stringstream stream( str ); \
			stream.imbue( std::locale::classic() ); \
			stream >> value; \
			return value; \
		}
#else
	#define DEFINE_STRING_TO_FLOAT( func, type ) DEFINE_STRING_TO_INT( func, type )
#endif

DEFINE_STRING_TO_INT( parseInt, long )
DEFINE_STRING_TO_INT( parseUInt, ulong )
DEFINE_STRING_TO_INT( parseInt64, int64 )
DEFINE_STRING_TO_INT( parseUInt64, uint64 )
DEFINE_STRING_TO_FLOAT( parseFloat, float )
DEFINE_STRING_TO_FLOAT( parseDouble, double )
DEFINE_STRING_TO_FLOAT( parseLongDouble, long double )

#endif

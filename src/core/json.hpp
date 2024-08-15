#ifndef CORE_JSON_HPP_
#define CORE_JSON_HPP_

#include <vector>
#include <variant>
#include <map>
#include <string>
#include <optional>
#include <iostream>
#include <type_traits>
#include <limits>
#include <cmath>
#include <stack>
#include <stdexcept>
#include "src/types.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/traceable-exception.hpp"

class Json;

typedef std::vector<Json> JArray;
typedef std::map<string,Json> JObject;

class JsonReaderException : public std::runtime_error, TraceableException {

	public:
	JsonReaderException( const string &reason ) :
		std::runtime_error( "Failed to parse JSON: " + reason ),
		TraceableException() {}

};

class JsonWriterException : public std::runtime_error, TraceableException {

	public:
	JsonWriterException() :
		std::runtime_error( "Failed to serialize to JSON." ),
		TraceableException() {}

	JsonWriterException( const string &reason ) :
		std::runtime_error( "Failed to serialize to JSON: " + reason ),
		TraceableException() {}

};

namespace JsonInternals {
	using std::string;

	struct JNull final {};
	struct JUndefined final {};
	typedef std::variant<int64,double,string,bool,JArray,JObject,JNull,JUndefined> JsonType;

	template<typename TT, typename TS> constexpr bool fits( TS value ) {
		if constexpr( std::is_floating_point_v<TT> || std::is_floating_point_v<TS> ) {
			return(
				value >= std::numeric_limits<TT>::min() &&
				value <= std::numeric_limits<TT>::max()
			);
		} else if constexpr( std::is_signed_v<TT> ) {
			if constexpr( std::is_signed_v<TS> ) {
				return(
					value >= std::numeric_limits<TT>::min() &&
					value <= std::numeric_limits<TT>::max()
				);
			} else {
				return value <= (std::make_unsigned_t<TT>)std::numeric_limits<TT>::max();
			}
		} else {
			return value >= 0 && (std::make_unsigned_t<TS>)value <= std::numeric_limits<TT>::max();
		}
	}

	enum class ContainerType {
		None,
		Object,
		Array
	};

	struct JsonWriterState {
		bool empty;
		bool expectPropertyName;
		ContainerType context;
	};

}

class Json final {
	private:
	const JsonInternals::JsonType m_value;

	static const Json *const UNDEFINED;

	public:
	template<typename T> T get() const;
	template<typename T> std::optional<T> tryGet() const;
	template<typename T> inline T getOrDefault( const T &defaultValue ) const {
		return tryGet<T>().value_or( defaultValue );
	}

	Json() : m_value( JsonInternals::JUndefined() ) {};
	Json( const JsonInternals::JsonType &value ) : m_value( value ) {};
	Json( const Json &other ) : m_value( other.m_value ) {};

	Json &operator=( Json &&other ) {
		new(this) Json( other );
		return *this;
	}

	inline const Json &operator[]( const std::string &property ) const {
		const JObject *maybeObject = std::get_if<JObject>( &m_value );
		if( maybeObject == nullptr ) {
			return *UNDEFINED;
		}
		auto propertyValue  = maybeObject->find( property );
		if( propertyValue == maybeObject->end() ) {
			return *UNDEFINED;
		}
		return propertyValue->second;
	}

	inline const Json &operator[]( int i ) const {
		const JArray *maybeArray = std::get_if<JArray>( &m_value );
		if( maybeArray == nullptr || i >= (int)maybeArray->size() ) {
			return *UNDEFINED;
		}
		return maybeArray->at( i );
	}

	inline const JArray &array() const {
		return std::get<JArray>( m_value );
	}

	inline const JObject &object() const {
		return std::get<JObject>( m_value );
	}

	inline bool exists() const {
		return !std::holds_alternative<JsonInternals::JUndefined>( m_value );
	}

	inline bool isNull() const {
		return std::holds_alternative<JsonInternals::JNull>( m_value );
	}

	inline bool isArray() const {
		return std::holds_alternative<JArray>( m_value );
	}

	inline bool isObject() const {
		return std::holds_alternative<JObject>( m_value );
	}

	inline bool hasValue() const {
		return exists() && !isNull();
	}

	static Json parse( std::istream &jsonStream );

};

template<typename T> T Json::get() const {
	if( std::holds_alternative<JsonInternals::JUndefined>( m_value ) ) {
		throw JsonReaderException( "Value does not exist in JSON." );
	}

	if constexpr( std::is_enum_v<T> ) {
		return (T)get<int64>();
	} else if constexpr( std::is_same_v<T,bool> ) {
		return std::get<bool>( m_value );
	} else if constexpr( std::is_integral_v<T> ) {
		const int64 *maybeLong = std::get_if<int64>( &m_value );
		if( maybeLong != nullptr ) {
			if( JsonInternals::fits<T>( *maybeLong ) ) {
				return (T)*maybeLong;
			} else throw JsonReaderException( "Number out of range of the requested type." );
		} else {
			const double value = std::get<double>( m_value );
			if( value != std::trunc( value ) ) {
				throw JsonReaderException( "Number is not an integer." );
			} if( JsonInternals::fits<T>( value ) ) {
				return (T)value;
			} else throw JsonReaderException( "Number out of range of the requested type." );
		}
	} else if constexpr( std::is_floating_point_v<T> ) {
		const double *maybeDouble = std::get_if<double>( &m_value );
		if( maybeDouble != nullptr ) {
			return (T)*maybeDouble;
		} else {
			return (T)std::get<int64>( m_value );
		}
	} else if constexpr( std::is_same_v<T,string> ) {
		return std::get<string>( m_value );
	} else {
		throw JsonReaderException( "Invalid Type" );
		//static_assert( false, "Invalid Type" );
	}
}

template<typename T> std::optional<T> Json::tryGet() const {
	std::optional<T> value;
	if( std::holds_alternative<JsonInternals::JUndefined>( m_value ) ) {
		return value;
	}

	if constexpr( std::is_enum_v<T> ) {
		const std::optional<int64> longVal = tryGet<int64>();
		return longVal.has_value() ? std::optional<T>( (T)longVal.value() ) : std::optional<T>();
	} else if constexpr( std::is_same_v<T,bool> ) {
		const bool *maybeBool = std::get_if<bool>( &m_value );
		if( maybeBool != nullptr ) {
			value = *maybeBool;
		}
	} else if constexpr( std::is_integral_v<T> ) {
		const int64 *maybeLong = std::get_if<int64>( &m_value );
		if( maybeLong != nullptr && JsonInternals::fits<T>( *maybeLong ) ) {
			value = (T)*maybeLong;
		} else {
			const double *maybeDouble = std::get_if<double>( &m_value );
			if( maybeDouble != nullptr && *maybeDouble == std::trunc( *maybeDouble ) && JsonInternals::fits<T>( *maybeDouble ) ) {
				value = (T)*maybeDouble;
			}
		}
	} else if constexpr( std::is_floating_point_v<T> ) {
		const double *maybeValue = std::get_if<double>( &m_value );
		if( maybeValue != nullptr ) {
			value = (T)*maybeValue;
		} else {
			const int64 *maybeLong = std::get_if<int64>( &m_value );
			if( maybeLong != nullptr ) {
				return (T)*maybeLong;
			}
		}
	} else if constexpr( std::is_same_v<T,std::string> ) {
		const std::string *maybeString = std::get_if<std::string>( &m_value );
		if( maybeString != nullptr ) {
			value = *maybeString;
		}
	} else {
		throw JsonReaderException( "Invalid Type" );
	}
	return value;
}

class JsonWriter final {
	private:
	std::ostream *const m_out;
	const bool m_pretty;
	std::stack<JsonInternals::JsonWriterState> m_state;

	void writeRawValue( const string &value );
	string quoteAndEscape( const string &value );

	public:
	JsonWriter( std::ostream *stream, bool humanReadable ) :
		m_out( stream ),
		m_pretty( humanReadable )
	{
		m_state.push({ true, false, JsonInternals::ContainerType::None });
	}

	inline void writeNull() {
		writeRawValue( "null" );
	}

	inline void writeBool( bool value ) {
		writeRawValue( value ? "true" : "false" );
	}

	inline void writeNumber( int64 value ) {
		writeRawValue( Number::toString( value ) );
	}

	inline void writeNumber( uint64 value ) {
		writeRawValue( Number::toString( value ) );
	}

	inline void writeNumber( double value ) {
		writeRawValue( Number::toString( value ) );
	}

	inline void writeString( const string &value ) {
		writeRawValue( quoteAndEscape( value ) );
	}

	void writeObjectStart();
	void writeObjectEnd();
	void writeArrayStart();
	void writeArrayEnd();

	void writePropertyName( const string &name );

	template<typename T> inline void writeProperty( const string &name, const T &value ) {
		writePropertyName( name );
		if constexpr( std::is_same_v<T,std::nullptr_t> ) {
			writeNull();
		} else if constexpr( std::is_same_v<T,bool> ) {
			writeBool( value );
		} else if constexpr( std::is_same_v<T,string> || std::is_same_v<T,char*> || std::is_same_v<T,const char*> ) {
			writeString( value );
		} else if constexpr( std::is_integral_v<T> && std::is_signed_v<T> ) {
			writeNumber( (int64)value );
		} else if constexpr( std::is_integral_v<T> && std::is_unsigned_v<T> ) {
			writeNumber( (uint64)value );
		} else if constexpr( std::is_floating_point_v<T> ) {
			writeNumber( (double)value );
		} else if constexpr( std::is_enum_v<T> ) {
			if( std::is_signed_v<std::underlying_type_t<T>> ) {
				writeNumber( (int64)value );
			} else {
				writeNumber( (uint64)value );
			}
		} else {
			throw JsonReaderException( "Invalid Type" );
		}
	}

};

namespace JsonSerializer {
	template<typename T> void serialize( JsonWriter&, const T& ) {
		throw JsonWriterException( "No JSON serializer has been registered for this type" );
	}
	template<typename T> T parse( const Json& ) {
		throw JsonReaderException( "No JSON deserializer has been registered for this type" );
	}
}

#endif /* CORE_JSON_HPP_ */

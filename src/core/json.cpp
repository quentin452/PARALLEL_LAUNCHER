#include "src/core/json.hpp"

#include <cctype>
#include <cstdlib>
#include <cstdio>

using namespace JsonInternals;
using namespace std;

const Json *const Json::UNDEFINED = new Json( JUndefined() );

inline static void notEof( istream &jsonStream ) {
	if( jsonStream.eof() ) {
		throw JsonReaderException( "Unexpected end of stream." );
	}
}

inline static void require( istream &jsonStream, char character ) {
	notEof( jsonStream );
	const char c = jsonStream.get();
	if( c != character ) {
		throw JsonReaderException( "Unexpected character '"s + c + "' (expected '" + character + "')." );
	}
}

inline static void skipWhitespace( istream &jsonStream ) {
	while( !jsonStream.eof() && isspace( jsonStream.peek() ) ) {
		jsonStream.get();
	}
}

inline static char parseHexCode( istream &jsonStream ) {
	char code[5];
	bool isNull = true;
	for( int i = 0; i < 4; i++ ) {
		notEof( jsonStream );
		const char c = jsonStream.get();
		if( !( c >= '0' && c <= '9' ) &&
			!( c >= 'a' && c <= 'f' ) &&
			!( c >= 'A' && c <= 'F' )
		) throw JsonReaderException( "Unexpected character '"s + c + "' (expected hex digit)." );
		code[i] = c;
		isNull &= ( c == '0' );
	}

	if( isNull ) {
		throw JsonReaderException( "Cannot have nul character in string." );
	}
	code[4] = '\0';

	return (char)strtol( code, nullptr, 16 );
}

static string parseString( istream &jsonStream ) {
	require( jsonStream, '"' );
	string text;
	while( !jsonStream.eof() ) {
		const char c = jsonStream.get();
		if( c == '"' ) {
			return text;
		} else if( c == '\\' ) {
			notEof( jsonStream );
			switch( char escapedChar = jsonStream.get() ) {
				case 'b': text += '\b'; break;
				case 'f': text += '\f'; break;
				case 'n': text += '\n'; break;
				case 'r': text += '\r'; break;
				case 't': text += '\t'; break;
				case 'u': text += parseHexCode( jsonStream ); break;
				default: text += escapedChar; break;
			}
		} else {
			text += c;
		}
	}
	throw JsonReaderException( "Unterminated string." );
}

static JObject parseObject( istream &jsonStream ) {
	require( jsonStream, '{' );
	JObject object;

	skipWhitespace( jsonStream );
	notEof( jsonStream );
	if( jsonStream.peek() == '}' ) {
		jsonStream.get();
		return object;
	}

	while( true ) {
		string propertyName = parseString( jsonStream );
		skipWhitespace( jsonStream );
		notEof( jsonStream );
		char colon = jsonStream.get();
		if( colon != ':' ) {
			throw JsonReaderException( "Unexpected character '"s + colon + "' (expected colon)." );
		}
		object[propertyName] = Json::parse( jsonStream );
		skipWhitespace( jsonStream );
		notEof( jsonStream );
		const char delim = jsonStream.get();
		if( delim == '}' ) {
			break;
		} else if( delim != ',' ) {
			throw JsonReaderException( "Unexpected character '"s + colon + "' (expected comma or close brace)." );
		}
		skipWhitespace( jsonStream );
	}
	return object;
}

static JArray parseArray( istream &jsonStream ) {
	require( jsonStream, '[' );
	JArray jArray;

	skipWhitespace( jsonStream );
	notEof( jsonStream );
	if( jsonStream.peek() == ']' ) {
		jsonStream.get();
		return jArray;
	}

	while( true ) {
		skipWhitespace( jsonStream );
		jArray.push_back( Json::parse( jsonStream ) );
		skipWhitespace( jsonStream );
		notEof( jsonStream );
		const char delimiter = jsonStream.get();
		if( delimiter == ']' ) {
			break;
		} else if( delimiter != ',' ) {
			throw JsonReaderException( "Unexpected character '"s + delimiter + "' (expected comma or close bracket)." );
		}
	}
	return jArray;
}

static constexpr bool isDigit( char c ) {
	return c >= '0' && c <= '9';
}

enum class NumberParseState {
	Sign,
	FirstDigit,
	Integer,
	Delimiter,
	FirstFractionDigit,
	Fraction,
	ExponentSign,
	FirstExponentDigit,
	Exponent
};

inline static Json parseNumber( istream &jsonStream ) {
	notEof( jsonStream );

	string numberString;
	NumberParseState state = NumberParseState::Sign;
	while( true ) {
		const char c = jsonStream.get();
		numberString += c;

		switch( state ) {
			case NumberParseState::Sign:
				if( c == '-' ) {
					state = NumberParseState::FirstDigit;
					break;
				}
				[[fallthrough]];
			case NumberParseState::FirstDigit:
				state = (c == '0') ? NumberParseState::Delimiter : NumberParseState::Integer;
				break;
			case NumberParseState::Integer:
				if( c == '.' ) {
					state = NumberParseState::FirstFractionDigit;
				} else if( c == 'e' || c == 'E' ) {
					state = NumberParseState::ExponentSign;
				} else if( !isDigit( c ) ) {
					throw JsonReaderException( "Invalid number format." );
				}
				break;
			case NumberParseState::Delimiter:
				if( c == '.' ) {
					state = NumberParseState::FirstFractionDigit;
				} else if( c == 'e' || c == 'E' ) {
					state = NumberParseState::ExponentSign;
				} else throw JsonReaderException( "Invalid number format." );
				break;
			case NumberParseState::FirstFractionDigit:
				if( !isDigit( c ) ) {
					throw JsonReaderException( "Invalid number format." );
				}
				state = NumberParseState::Fraction;
				break;
			case NumberParseState::Fraction:
				if( c == 'e' || c == 'E' ) {
					state = NumberParseState::ExponentSign;
				}
				break;
			case NumberParseState::ExponentSign:
				if( c == '+' || c == '-' ) {
					state = NumberParseState::FirstExponentDigit;
					break;
				} else if( isDigit( c ) ) {
					state = NumberParseState::Exponent;
					break;
				} else throw JsonReaderException( "Invalid number format." );
			case NumberParseState::FirstExponentDigit:
				if( !isDigit( c ) ) {
					throw JsonReaderException( "Invalid number format." );
				}
				state = NumberParseState::Exponent;
				break;
			case NumberParseState::Exponent:
				if( !isDigit( c ) ) {
					throw JsonReaderException( "Invalid number format." );
				}
				break;
			default:
				throw JsonReaderException( "Internal Error." );
		}

		if( jsonStream.eof() ) {
			break;
		} else {
			const char nextChar = jsonStream.peek();
			if( isspace( nextChar ) || nextChar == ']' || nextChar == '}' || nextChar == ',' ) {
				break;
			}
		}
	}

	if( state == NumberParseState::Fraction || state == NumberParseState::Exponent ) {
		return Json( Number::parseDouble( numberString ) );
	} else if( state == NumberParseState::Integer || state == NumberParseState::Delimiter ) {
		return Json( Number::parseInt64( numberString ) );
	} else throw JsonReaderException( "Invalid number format." );
}

inline static bool parseBoolean( istream &jsonStream ) {
	const char firstChar = jsonStream.get();
	if( firstChar == 't' ) {
		require( jsonStream, 'r' );
		require( jsonStream, 'u' );
		require( jsonStream, 'e' );
		return true;
	} else if( firstChar == 'f' ) {
		require( jsonStream, 'a' );
		require( jsonStream, 'l' );
		require( jsonStream, 's' );
		require( jsonStream, 'e' );
		return false;
	} else throw JsonReaderException( "Unexpected character '"s + firstChar + "' (expected boolean)." );
}

inline static JNull parseNull( istream &jsonStream ) {
	require( jsonStream, 'n' );
	require( jsonStream, 'u' );
	require( jsonStream, 'l' );
	require( jsonStream, 'l' );
	return JNull();
}

Json Json::parse( istream &jsonStream ) {
	skipWhitespace( jsonStream );
	notEof( jsonStream );

	switch( char token = jsonStream.peek() ) {
		case '{':
			return Json( parseObject( jsonStream ) );
		case '[':
			return Json( parseArray( jsonStream ) );
		case '"':
			return Json( parseString( jsonStream ) );
		case 't':
		case 'f':
			return Json( parseBoolean( jsonStream ) );
		case 'n':
			return Json( parseNull( jsonStream ) );
		default:
			if( token == '-' || ( token >= '0' && token <= '9' ) ) {
				return parseNumber( jsonStream );
			}
			throw JsonReaderException( "Unexpected character '"s + token + "' (expected value)." );
	}
}

inline static void indent( ostream *stream, int tabs ) {
	for( int i = 0; i < tabs; i++ ) {
		stream->put( '\t' );
	}
}

string JsonWriter::quoteAndEscape( const string &value ) {
	string qstr = "\"";
	qstr.reserve( value.size() + 3 );
	int mcbr = 0;
	for( char c : value ) {
		switch( c ) {
			case '"':
				qstr += "\\\"";
				break;
			case '\\':
				qstr += "\\\\";
				break;
			case '\b':
				qstr += "\\b";
				break;
			case '\f':
				qstr += "\\f";
				break;
			case '\n':
				qstr += "\\n";
				break;
			case '\r':
				qstr += "\\r";
				break;
			case '\t':
				qstr += "\\t";
				break;
			default:
				if( mcbr > 0 ) {
					qstr += c;
					mcbr--;
				} else if( (ubyte)c & 0x80 ) {
					if( (ubyte)c & 0xF0 ) {
						mcbr = 3;
					} else if( (ubyte)c & 0xE0 ) {
						mcbr = 2;
					} else {
						mcbr = 1;
					}
					qstr += c;
				} else if( isprint( c ) ) {
					qstr += c;
				} else {
					qstr += "\\u";
					char hexCode[5];
					snprintf( hexCode, 5, "%04X", (ubyte)c );
					qstr.append( hexCode, 4 );
				}
		}
	}
	qstr += '"';
	return qstr;
}

void JsonWriter::writeRawValue( const string &value ) {
	if( m_state.top().expectPropertyName ) {
		throw JsonWriterException();
	}

	JsonWriterState &state = m_state.top();
	switch( state.context ) {
		case ContainerType::None:
			if( !state.empty ) {
				throw JsonWriterException();
			}
			state.empty = false;
			m_out->write( value.c_str(), value.size() );
			break;
		case ContainerType::Object:
			if( state.expectPropertyName ) {
				throw JsonWriterException();
			}
			m_out->write( value.c_str(), value.size() );
			state.expectPropertyName = true;
			break;
		case ContainerType::Array:
			if( !state.empty ) {
				m_out->put( ',' );
			}
			state.empty = false;
			if( m_pretty ) {
				m_out->put( '\n' );
				indent( m_out, (int)m_state.size() - 1 );
			}
			m_out->write( value.c_str(), value.size() );
			break;
		default:
			throw JsonWriterException( "Internal Error" );
	}
}

void JsonWriter::writePropertyName( const string &name ) {
	JsonWriterState &objectState = m_state.top();
	if( objectState.context != ContainerType::Object || !objectState.expectPropertyName ) {
		throw JsonWriterException();
	}

	if( !objectState.empty ) {
		m_out->put( ',' );
	}
	objectState.expectPropertyName = false;
	objectState.empty = false;
	if( m_pretty ) {
		m_out->put( '\n' );
		indent( m_out, (int)m_state.size() - 1 );
	}

	const string &propertyName = quoteAndEscape( name );
	m_out->write( propertyName.c_str(), propertyName.size() );
	m_out->put( ':' );
	if( m_pretty ) {
		m_out->put( ' ' );
	}
}

void JsonWriter::writeObjectStart() {
	writeRawValue( "{" );
	m_state.push({ true, true, ContainerType::Object });
}

void JsonWriter::writeObjectEnd() {
	const JsonWriterState &objectState = m_state.top();
	if( objectState.context != ContainerType::Object || !objectState.expectPropertyName ) {
		throw JsonWriterException();
	}

	if( !m_pretty || objectState.empty ) {
		m_out->put( '}' );
		m_state.pop();
		return;
	}

	m_state.pop();
	m_out->put( '\n' );
	indent( m_out, (int)m_state.size() - 1 );
	m_out->put( '}' );
}

void JsonWriter::writeArrayStart() {
	writeRawValue( "[" );
	m_state.push({ true, false, ContainerType::Array });
}

void JsonWriter::writeArrayEnd() {
	const JsonWriterState &arrayState = m_state.top();
	if( arrayState.context != ContainerType::Array ) {
		throw JsonWriterException();
	}

	if( !m_pretty || arrayState.empty ) {
		m_out->put( ']' );
		m_state.pop();
		return;
	}

	m_state.pop();
	m_out->put( '\n' );
	indent( m_out, (int)m_state.size() - 1 );
	m_out->put( ']' );
}

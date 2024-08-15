#ifndef SRC_DB_SQLITE_HPP_
#define SRC_DB_SQLITE_HPP_

#include <vector>
#include <array>
#include <sqlite3.h>
#include "src/core/filesystem.hpp"
#include "src/core/uuid.hpp"
#include "src/core/rom.hpp"
#include "src/types.hpp"

#define DATA_COPY true
#define DATA_REF false

class DataRecordIterator final {

	private:
	sqlite3_stmt *m_statement;
	int m_status;

	public:
	DataRecordIterator( sqlite3_stmt *statement ) noexcept;
	DataRecordIterator( DataRecordIterator &&other ) noexcept;
	DataRecordIterator( const DataRecordIterator &other ) = delete;
	~DataRecordIterator() noexcept;

	bool moveNext() noexcept;
	void close() noexcept;

	inline int status() const noexcept { return m_status; }
	inline bool good() const noexcept {
		return m_status == SQLITE_DONE || m_status == SQLITE_ROW || m_status == SQLITE_OK;
	}

	inline int isNull( int index ) const noexcept {
		return sqlite3_column_type( m_statement, index ) == SQLITE_NULL;
	}

	inline int getInt( int index ) const noexcept {
		return sqlite3_column_int( m_statement, index );
	}

	inline int64 getLong( int index ) const noexcept {
		return sqlite3_column_int64( m_statement, index );
	}

	inline double getFloat( int index ) const noexcept {
		return sqlite3_column_double( m_statement, index );
	}

	inline string getString( int index ) const {
		return string( (const char*)sqlite3_column_text( m_statement, index ) );
	}

	inline Uuid getUuid( int index ) const noexcept {
		return Uuid( (const ubyte*)sqlite3_column_blob( m_statement, index ) );
	}

	inline bool getBool( int index ) const noexcept { return getInt( index ) != 0; }
	inline uint getUInt( int index ) const noexcept { return (uint)getLong( index ); }
	inline fs::path getPath( int index ) const { return fs::to_path( getString( index ) ); }

	inline uint64 getULong( int index ) const noexcept {
		const int64 value = getLong( index );
		return reinterpret_cast<const uint64&>( value );
	}

	inline N64InputTypes getInputTypes( int index ) const noexcept {
		const N64InputType *data = (const N64InputType*)sqlite3_column_blob( m_statement, index );
		return{ data[0], data[1], data[2], data[3] };
	}

};

class SqlCommand final {

	private:
	sqlite3_stmt *m_statement;
	int m_bindingIndex;

	public:
	SqlCommand( const char *sql ) noexcept;
	SqlCommand( SqlCommand &&other ) noexcept;
	SqlCommand( const SqlCommand &other ) = delete;
	~SqlCommand() noexcept;

	void execNonQuery( int *status = nullptr ) noexcept;

	[[nodiscard]] inline DataRecordIterator execQuery() {
		m_bindingIndex = 0;
		return DataRecordIterator( m_statement );
	}

	inline int addParameter( int value ) noexcept {
		return sqlite3_bind_int( m_statement, ++m_bindingIndex, value );
	}

	inline int addParameter( int64 value ) noexcept {
		return sqlite3_bind_int64( m_statement, ++m_bindingIndex, value );
	}

	inline int addParameter( double value ) noexcept {
		return sqlite3_bind_double( m_statement, ++m_bindingIndex, value );
	}

	inline int addParameter( const char *value, bool copy ) noexcept {
		return sqlite3_bind_text( m_statement, ++m_bindingIndex, value, -1, copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
	}

	inline int addParameter( const string &value, bool copy ) noexcept {
		return sqlite3_bind_text( m_statement, ++m_bindingIndex, value.c_str(), (int)value.length(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
	}

	inline int addParameter( const Uuid &value, bool copy ) noexcept {
		return sqlite3_bind_blob( m_statement, ++m_bindingIndex, (const void*)value.data(), 16, copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
	}

	inline int addParameter( const N64InputTypes &value, bool copy ) noexcept {
		return sqlite3_bind_blob( m_statement, ++m_bindingIndex, (const void*)value.data(), sizeof( N64InputTypes ), copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
	}

	inline int addParameter( bool value ) noexcept { return addParameter( (int)value ); }
	inline int addParameter( ubyte value ) noexcept { return addParameter( (int)value ); }
	inline int addParameter( ushort value ) noexcept { return addParameter( (int)value ); }
	inline int addParameter( uint value ) noexcept { return addParameter( (int64)value ); }

	inline int addParameter( uint64 value ) noexcept { return addParameter( reinterpret_cast<int64&>( value ) ); }

};

class SqlBatch final {

	private:
	std::vector<sqlite3_stmt*> m_statements;
	int m_bindingIndex;

	public:
	SqlBatch( const char *sql );
	SqlBatch( SqlBatch &&other ) noexcept;
	SqlBatch( const SqlBatch &other ) = delete;
	~SqlBatch();

	void execNonQuery() noexcept;

	inline void addParameter( int value ) noexcept {
		m_bindingIndex++;
		for( sqlite3_stmt *statement : m_statements ) {
			sqlite3_bind_int( statement, m_bindingIndex, value );
		}
	}

	inline void addParameter( int64 value ) noexcept {
		m_bindingIndex++;
		for( sqlite3_stmt *statement : m_statements ) {
			sqlite3_bind_int64( statement, m_bindingIndex, value );
		}
	}

	inline void addParameter( double value ) noexcept {
		m_bindingIndex++;
		for( sqlite3_stmt *statement : m_statements ) {
			sqlite3_bind_double( statement, m_bindingIndex, value );
		}
	}

	inline void addParameter( const char *value, bool copy ) noexcept {
		m_bindingIndex++;
		for( sqlite3_stmt *statement : m_statements ) {
			sqlite3_bind_text( statement, m_bindingIndex, value, -1, copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
		}
	}

	inline void addParameter( const string &value, bool copy ) noexcept {
		m_bindingIndex++;
		for( sqlite3_stmt *statement : m_statements ) {
			sqlite3_bind_text( statement, m_bindingIndex, value.c_str(), (int)value.length(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
		}
	}

	inline void addParameter( const Uuid &value, bool copy ) noexcept {
		m_bindingIndex++;
		for( sqlite3_stmt *statement : m_statements ) {
			sqlite3_bind_blob( statement, m_bindingIndex, (const void*)value.data(), 16, copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
		}
	}

	inline void addParameter( const N64InputTypes &value, bool copy ) noexcept {
		m_bindingIndex++;
		for( sqlite3_stmt *statement : m_statements ) {
			sqlite3_bind_blob( statement, m_bindingIndex, (const void*)value.data(), sizeof( N64InputTypes ), copy ? SQLITE_TRANSIENT : SQLITE_STATIC );
		}
	}

	inline void addParameter( bool value ) noexcept { addParameter( (int)value ); }
	inline void addParameter( ubyte value ) noexcept { addParameter( (int)value ); }
	inline void addParameter( ushort value ) noexcept { addParameter( (int)value ); }
	inline void addParameter( uint value ) noexcept { addParameter( (int64)value ); }

	inline void addParameter( uint64 value ) noexcept { addParameter( reinterpret_cast<int64&>( value ) ); }

};

#endif /* SRC_DB_SQLITE_HPP_ */

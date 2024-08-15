#include "src/db/sqlite.hpp"
#include "src/db/transaction.hpp"

#include "src/core/numeric-string.hpp"
#include "src/core/thread-safety.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/crash.hpp"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

inline static void assertStatusGood( sqlite3 *connection, int status ) {
	if( status == SQLITE_DONE || status == SQLITE_ROW || status == SQLITE_OK ) return;

	const int extStatus = sqlite3_extended_errcode( connection );
	string errorMessage = "SQL Error (Status = "s + Number::toString( status ) + "." + Number::toString( extStatus ) + ")\n";
	errorMessage += sqlite3_errmsg( connection );
	pl_crash( "Assertion Failed", errorMessage.c_str() );
}

static const char s_initSql[] = R"#(
PRAGMA encoding = "UTF-8";
PRAGMA locking_mode=NORMAL;
PRAGMA journal_mode=WAL;
PRAGMA synchronous=NORMAL;

CREATE TABLE IF NOT EXISTS ROMS(
	sha1 TEXT NOT NULL PRIMARY KEY,
	name TEXT NOT NULL,
	internal_name TEXT NOT NULL,
	emulator INTEGER NOT NULL,
	parallel_plugin INTEGER NOT NULL,
	mupen_plugin INTEGER NOT NULL,
	upscale_texrects INTEGER NOT NULL,
	remove_borders INTEGER NOT NULL,
	emulate_framebuffer INTEGER NOT NULL,
	n64_depth_compare INTEGER NOT NULL,
	gliden_lle_rsp INTEGER NOT NULL,
	overclock_cpu INTEGER NOT NULL,
	overclock_vi INTEGER NOT NULL,
	widescreen INTEGER NOT NULL,
	big_eeprom INTEGER NOT NULL,
	use_interpreter INTEGER NOT NULL,
	input_mode_id BLOB NOT NULL,
	last_played INTEGER NOT NULL,
	play_time INTEGER NOT NULL,
	crc32 INTEGER NOT NULL
);
CREATE INDEX IF NOT EXISTS ix_rom_by_crc ON ROMS( crc32 );

CREATE TABLE IF NOT EXISTS ROM_PATHS(
	path TEXT NOT NULL PRIMARY KEY,
	last_modified INTEGER NOT NULL,
	sha1 TEXT NOT NULL,
	local INTEGER NOT NULL
);
CREATE INDEX IF NOT EXISTS ix_paths_by_sha1 ON ROM_PATHS( sha1 );

CREATE TABLE IF NOT EXISTS ROM_GROUPS(
	sha1 TEXT NOT NULL,
	group_name TEXT NOT NULL,
	PRIMARY KEY( sha1, group_name )
);
CREATE INDEX IF NOT EXISTS ix_groups_by_name ON ROM_GROUPS( group_name );

CREATE VIEW IF NOT EXISTS TAGGED_ROMS AS
SELECT ROMS.*, group_concat( group_name, x'0A' ) AS groups
FROM ROMS LEFT JOIN ROM_GROUPS USING( sha1 )
GROUP BY sha1;

CREATE TABLE IF NOT EXISTS PERSISTENT_GROUPS(
	name TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS MANUAL_ROM_PATHS(
	path TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS RHDC_HACKS(
	id TEXT NOT NULL PRIMARY KEY,
	name TEXT NOT NULL,
	slug TEXT NOT NULL,
	description TEXT NOT NULL,
	star_count INTEGER NOT NULL,
	downloads INTEGER NOT NULL,
	avg_rating REAL NOT NULL,
	avg_difficulty REAL NOT NULL,
	category TEXT NOT NULL,
	submit_starpower INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS RHDC_HACK_PROGRESS(
	id TEXT NOT NULL PRIMARY KEY,
	my_rating INTEGER NOT NULL,
	my_difficulty INTEGER NOT NULL,
	stars_collected INTEGER NOT NULL,
	completed INTEGER NOT NULL
);

CREATE TABLE IF NOT EXISTS RHDC_HACK_VERSIONS(
	sha1 TEXT NOT NULL PRIMARY KEY,
	id TEXT NOT NULL,
	version_name TEXT NOT NULL,
	sort_order INTEGER NOT NULL,
	recommended_plugin INTEGER NOT NULL,
	hack_flags INTEGER NOT NULL,
	archived INTEGER NOT NULL
);
CREATE INDEX IF NOT EXISTS ix_hack_versions_by_id ON RHDC_HACK_VERSIONS( id );

CREATE TABLE IF NOT EXISTS RHDC_HACK_AUTHORS(
	id TEXT NOT NULL,
	author TEXT NOT NULL,
	PRIMARY KEY( id, author )
);
CREATE INDEX IF NOT EXISTS ix_hack_authors ON RHDC_HACK_AUTHORS( author );

CREATE TABLE IF NOT EXISTS RHDC_LAST_PLAYED_VERSIONS(
	id TEXT NOT NULL PRIMARY KEY,
	sha1 TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS LOGS(
	level INTEGER NOT NULL,
	timestamp INTEGER NOT NULL,
	message TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS PENDING_PLUGIN_CHECKS(
	sha1 TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS GROUP_SAVES(
	hack_id TEXT NOT NULL,
	sha1 TEXT NOT NULL,
	PRIMARY KEY( hack_id, sha1 )
);
CREATE INDEX IF NOT EXISTS ix_group_saves_rl ON GROUP_SAVES( sha1, hack_id );

CREATE TABLE IF NOT EXISTS DISABLED_GROUP_SAVES(
	hack_id TEXT NOT NULL PRIMARY KEY
);

CREATE TABLE IF NOT EXISTS LIBPL_SD_CARDS(
	uid VARCHAR(36) NOT NULL PRIMARY KEY,
	name VARCHAR(16) NOT NULL
);

CREATE TABLE IF NOT EXISTS CRASH_LOGS(
	crash_date INTEGER NOT NULL PRIMARY KEY,
	report TEXT NOT NULL,
	sent_at INTEGER
);
CREATE INDEX IF NOT EXISTS ix_unreported_crash_logs ON CRASH_LOGS( crash_date ) WHERE sent_at IS NULL;

)#";

class DbConnection final {

	private:
	sqlite3 *m_connection;

	inline DbConnection( sqlite3 *connection ) noexcept : m_connection( connection ) {}
	inline DbConnection( DbConnection &&other ) noexcept : m_connection( other.m_connection ) { other.m_connection = nullptr; }
	DbConnection( const DbConnection &other ) = delete;
	~DbConnection() noexcept {
		if( m_connection != nullptr ) sqlite3_close( m_connection );
	}

	static DbConnection create() noexcept {
		const fs::path dbFile = BaseDir::data() / _NFS("roms.sqlite");

		sqlite3 *db = nullptr;
		int status = sqlite3_open( dbFile.u8string().c_str(), &db );
		if( status != SQLITE_OK ) {
			std::cerr << "Failed to open or create sqlite database" << std::endl << std::flush;
			std::exit( status );
		}

		sqlite3_extended_result_codes( db, 1 );
		sqlite3_busy_timeout( db, 280 );

		for( int i = 0; i < 3; i++ ) {
			status = sqlite3_exec( db, s_initSql, nullptr, nullptr, nullptr );
			if( status != SQLITE_BUSY ) break;
			std::this_thread::sleep_for( 75ms );
		}

		assertStatusGood( db, status );
		return DbConnection( db );
	}

	public:
	static sqlite3 *instance() noexcept {
		static DbConnection s_db = DbConnection::create();
		return s_db.m_connection;
	}

};

SqlCommand::SqlCommand( const char *sql ) noexcept : m_bindingIndex( 0 ) {
	const int status = sqlite3_prepare_v3(
		DbConnection::instance(),
		sql,
		-1,
		SQLITE_PREPARE_PERSISTENT,
		&m_statement,
		nullptr
	);
	assertStatusGood( DbConnection::instance(), status );
}

SqlCommand::SqlCommand( SqlCommand &&other ) noexcept :
	m_statement( other.m_statement ),
	m_bindingIndex( other.m_bindingIndex )
{
	other.m_statement = nullptr;
}

SqlCommand::~SqlCommand() noexcept {
	sqlite3_finalize( m_statement );
}

void SqlCommand::execNonQuery( int *status ) noexcept {
	ThreadSafety::assertThreadIsSafe();
	m_bindingIndex = 0;

	const int sqlStatus = sqlite3_step( m_statement );
	sqlite3_reset( m_statement );
	sqlite3_clear_bindings( m_statement );
	if( status != nullptr ) {
		*status = sqlStatus;
	} else {
		assertStatusGood( DbConnection::instance(), sqlStatus );
	}

}

DataRecordIterator::DataRecordIterator( sqlite3_stmt *statement ) noexcept :
	m_statement( statement ),
	m_status( SQLITE_OK )
{
	ThreadSafety::assertThreadIsSafe();
}

DataRecordIterator::DataRecordIterator( DataRecordIterator &&other ) noexcept :
	m_statement( other.m_statement ),
	m_status( other.m_status )
{
	ThreadSafety::assertThreadIsSafe();
	other.m_statement = nullptr;
	other.m_status = SQLITE_MISUSE;
}

DataRecordIterator::~DataRecordIterator() noexcept {
	if( m_statement == nullptr ) return;
	sqlite3_reset( m_statement );
	sqlite3_clear_bindings( m_statement );
	assertStatusGood( DbConnection::instance(), status() );
}

bool DataRecordIterator::moveNext() noexcept {
	m_status = sqlite3_step( m_statement );
	return m_status == SQLITE_ROW;
}

void DataRecordIterator::close() noexcept {
	sqlite3_reset( m_statement );
	sqlite3_clear_bindings( m_statement );
	m_statement = nullptr;
	m_status = SQLITE_MISUSE;
}

SqlBatch::SqlBatch( const char *sql ) : m_bindingIndex( 0 ) {
	while( sql[0] != '\0' ) {
		if( sql[0] == ' ' || sql[0] == '\t' || sql[0] == '\n' ) {
			sql++;
			continue;
		}

		const char *next = nullptr;
		sqlite3_stmt *statement = nullptr;
		const int status = sqlite3_prepare_v3(
			DbConnection::instance(),
			sql,
			-1,
			SQLITE_PREPARE_PERSISTENT,
			&statement,
			&next
		);
		assertStatusGood( DbConnection::instance(), status );
		sql = next;
		m_statements.push_back( statement );
	}
	pl_assert( m_statements.size() > 1 );
}

SqlBatch::SqlBatch( SqlBatch &&other ) noexcept :
	m_statements( std::move( other.m_statements ) ),
	m_bindingIndex( other.m_bindingIndex ) {}

SqlBatch::~SqlBatch() {
	for( sqlite3_stmt *statement : m_statements ) {
		sqlite3_finalize( statement );
	}
}

void SqlBatch::execNonQuery() noexcept {
	static SqlCommand s_start( "SAVEPOINT batch" );
	static SqlCommand s_end( "RELEASE batch" );

	m_bindingIndex = 0;
	s_start.execNonQuery();
	for( sqlite3_stmt *statement : m_statements ) {
		const int sqlStatus = sqlite3_step( statement );
		sqlite3_reset( statement );
		sqlite3_clear_bindings( statement );
		assertStatusGood( DbConnection::instance(), sqlStatus );
	}
	s_end.execNonQuery();
}

SqlTransaction::SqlTransaction() noexcept : m_committed( false ) {
	static SqlCommand s_beginSql( "BEGIN TRANSACTION" );
	s_beginSql.execNonQuery();
}

SqlTransaction::SqlTransaction( SqlTransaction &&other ) noexcept : m_committed( other.m_committed ) {
	other.m_committed = true;
}

SqlTransaction::~SqlTransaction() noexcept {
	if( m_committed ) return;
	static SqlCommand s_rollbackSql( "ROLLBACK TRANSACTION" );
	s_rollbackSql.execNonQuery();
}

void SqlTransaction::commit() noexcept {
	pl_assert( !m_committed );
	static SqlCommand s_commitSql( "COMMIT TRANSACTION" );
	s_commitSql.execNonQuery();
	m_committed = true;
}

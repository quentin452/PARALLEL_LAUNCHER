#include "src/db/data-provider.hpp"

#include "src/db/sqlite.hpp"
#include "src/core/special-groups.hpp"

static string escapePattern( const string &search ) {
	string escapedSearch = "%";
	escapedSearch.reserve( search.length() + 2 );

	for( const char &c : search ) {
		if( c == '%' || c == '_' || c == '\\' ) {
			escapedSearch += '\\';
		}
		escapedSearch += c;
	}

	escapedSearch += '%';
	return escapedSearch;
}

void DataProvider::addRomInfo( const RomInfo &romInfo ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO ROMS(
	sha1,
	name,
	internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	upscale_texrects,
	remove_borders,
	emulate_framebuffer,
	n64_depth_compare,
	gliden_lle_rsp,
	overclock_cpu,
	overclock_vi,
	widescreen,
	big_eeprom,
	use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	crc32,
	input_types,
	sd_card
) VALUES (
	?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20, ?21, ?22
)
)#"
	);

	s_cmd.addParameter( romInfo.sha1, DATA_REF );
	s_cmd.addParameter( romInfo.name, DATA_REF );
	s_cmd.addParameter( romInfo.internalName, DATA_REF );
	s_cmd.addParameter( (int)romInfo.emulator );
	s_cmd.addParameter( (int)romInfo.parallelPlugin );
	s_cmd.addParameter( (int)romInfo.mupenPlugin );
	s_cmd.addParameter( romInfo.parallelTexRectUpscaling );
	s_cmd.addParameter( romInfo.parallelRemoveBorders );
	s_cmd.addParameter( romInfo.glidenFramebufferEmulation );
	s_cmd.addParameter( romInfo.glidenCorrectDepthCompare );
	s_cmd.addParameter( romInfo.glidenParallelRsp );
	s_cmd.addParameter( romInfo.overclockCPU );
	s_cmd.addParameter( romInfo.overclockVI );
	s_cmd.addParameter( (int)romInfo.widescreen );
	s_cmd.addParameter( romInfo.bigEEPROM );
	s_cmd.addParameter( romInfo.useInterpreter );
	s_cmd.addParameter( romInfo.inputModeId, DATA_REF );
	s_cmd.addParameter( romInfo.lastPlayed );
	s_cmd.addParameter( romInfo.playTime );
	s_cmd.addParameter( (int64)romInfo.crc32 );
	s_cmd.addParameter( romInfo.inputTypes, DATA_REF );
	s_cmd.addParameter( romInfo.sdCard, DATA_REF );
	s_cmd.execNonQuery();

	for( const string &group : romInfo.groups ) {
		static SqlCommand s_cmd2( R"#(
INSERT OR IGNORE INTO ROM_GROUPS( sha1, group_name )
VALUES( ?1, ?2 )
)#"
		);

		s_cmd2.addParameter( romInfo.sha1, DATA_REF );
		s_cmd2.addParameter( group, DATA_REF );
		s_cmd2.execNonQuery();
	}
}

void DataProvider::addRomFile( const RomFile &romFile ) {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO ROM_PATHS( path, last_modified, sha1, local )
VALUES( ?1, ?2, ?3, ?4 )
)#"
	);

	s_cmd.addParameter( romFile.path.u8string(), DATA_COPY );
	s_cmd.addParameter( romFile.lastModified );
	s_cmd.addParameter( romFile.sha1, DATA_REF );
	s_cmd.addParameter( romFile.local );
	s_cmd.execNonQuery();
}

void DataProvider::addGroup( const string &groupName ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO PERSISTENT_GROUPS( name )
VALUES( ?1 )
)#"
	);

	s_cmd.addParameter( groupName, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::deleteGroup( const string &groupName ) noexcept {
	static SqlBatch s_cmd( R"#(
DELETE FROM PERSISTENT_GROUPS 
WHERE name = ?1;

DELETE FROM ROM_GROUPS
WHERE group_name = ?1;
)#"
	);

	s_cmd.addParameter( groupName, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::renameGroup( const string &oldName, const string &newName ) noexcept {
	static SqlBatch s_cmd( R"#(
UPDATE PERSISTENT_GROUPS
SET name = ?2
WHERE name = ?1;

UPDATE ROM_GROUPS
SET group_name = ?2
WHERE group_name = ?1;
)#"
	);

	s_cmd.addParameter( oldName, DATA_REF );
	s_cmd.addParameter( newName, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomHeaderInfo( const string &sha1, const string &name, uint crc32, const N64InputTypes &inputTypes ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS SET
	internal_name = ?2,
	crc32 = ?3,
	input_types = ?4
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( name, DATA_REF );
	s_cmd.addParameter( crc32 );
	s_cmd.addParameter( inputTypes, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomEmulator( const string &sha1, EmulatorCore core ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET emulator = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( (int)core );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomParallelPlugin( const string &sha1, GfxPlugin plugin ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET parallel_plugin = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( (int)plugin );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomMupenPlugin( const string &sha1, GfxPlugin plugin ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET mupen_plugin = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( (int)plugin );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomCpuOverclocking( const string &sha1, bool overclock ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET overclock_cpu = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( overclock );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomViOverclocking( const string &sha1, bool overclock ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET overclock_vi = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( overclock );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomWidescreen( const string &sha1, WidescreenMode widescreen ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET widescreen = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( (int)widescreen );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomBigEeprom( const string &sha1, bool bigEeprom ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET big_eeprom = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( bigEeprom );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomUseInterpreter( const string &sha1, bool useInterpreter ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET use_interpreter = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( useInterpreter );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomInputMode( const string &sha1, const Uuid &inputModeId ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET input_mode_id = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( inputModeId, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::updatePlayTime(
	const string &sha1,
	int64 lastPlayed,
	int64 playTime
) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS SET
	last_played = ?2,
	play_time = ?3
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( lastPlayed );
	s_cmd.addParameter( playTime );
	s_cmd.execNonQuery();
}

void DataProvider::updateSdCard( const string &sha1, const string &card ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET sd_card = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( card, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::renameRom( const string &sha1, const string &name ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET name = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( name, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::updatePluginSettings(
	const string &sha1,
	EmulatorCore core,
	GfxPlugin parallelPlugin,
	GfxPlugin mupenPlugin,
	bool upscaleTexrects,
	bool removeBorders,
	bool emulateFramebuffer,
	bool accurateDepthCompare,
	bool glidenParallelRsp
) {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS SET
	emulator = ?2,
	parallel_plugin = ?3,
	mupen_plugin = ?4,
	upscale_texrects = ?5,
	remove_borders = ?6,
	emulate_framebuffer = ?7,
	n64_depth_compare = ?8,
	gliden_lle_rsp = ?9
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( (int)core );
	s_cmd.addParameter( (int)parallelPlugin );
	s_cmd.addParameter( (int)mupenPlugin );
	s_cmd.addParameter( upscaleTexrects );
	s_cmd.addParameter( removeBorders );
	s_cmd.addParameter( emulateFramebuffer );
	s_cmd.addParameter( accurateDepthCompare );
	s_cmd.addParameter( glidenParallelRsp );
	s_cmd.execNonQuery();
}

static void addGroups( std::set<string> &groups, const string &groupsString ) {
	size_t nameStart = 0;
	for( size_t i = 0; i < groupsString.length(); i++ ) {
		if( groupsString[i] != '\n' ) continue;
		if( i != nameStart ) {
			groups.insert( groupsString.substr( nameStart, i - nameStart ) );
		}
		nameStart = i + 1;
	}

	if( nameStart < groupsString.length() ) {
		groups.insert( groupsString.substr( nameStart ) );
	}
}

bool DataProvider::tryFetchRomByHash( const string &sha1, bool includeGroups, RomInfo *romInfo ) noexcept {
	static SqlCommand s_cmdWithoutGroups( R"#(
SELECT name, internal_name, emulator, parallel_plugin, mupen_plugin, upscale_texrects, remove_borders, emulate_framebuffer, n64_depth_compare, gliden_lle_rsp, overclock_cpu, overclock_vi, widescreen, big_eeprom, use_interpreter, input_mode_id, last_played, play_time, crc32, input_types, sd_card
FROM ROMS
WHERE sha1 = ?1
)#"
	);

	static SqlCommand s_cmdWithGroups( R"#(
SELECT name, internal_name, emulator, parallel_plugin, mupen_plugin, upscale_texrects, remove_borders, emulate_framebuffer, n64_depth_compare, gliden_lle_rsp, overclock_cpu, overclock_vi, widescreen, big_eeprom, use_interpreter, input_mode_id, last_played, play_time, crc32, input_types, sd_card, groups
FROM TAGGED_ROMS
WHERE sha1 = ?1
)#"
	);

	SqlCommand &cmd = (includeGroups && romInfo != nullptr) ? s_cmdWithGroups : s_cmdWithoutGroups;
	cmd.addParameter( sha1, DATA_REF );

	DataRecordIterator results = cmd.execQuery();
	if( !results.moveNext() ) return false;

	if( romInfo != nullptr ) {
		romInfo->sha1 = sha1;
		romInfo->name = results.getString( 0 );
		romInfo->internalName = results.getString( 1 );
		romInfo->emulator = (EmulatorCore)results.getInt( 2 );
		romInfo->parallelPlugin = (GfxPlugin)results.getInt( 3 );
		romInfo->mupenPlugin = (GfxPlugin)results.getInt( 4 );
		romInfo->parallelTexRectUpscaling = results.getBool( 5 );
		romInfo->parallelRemoveBorders = results.getBool( 6 );
		romInfo->glidenFramebufferEmulation = results.getBool( 7 );
		romInfo->glidenCorrectDepthCompare = results.getBool( 8 );
		romInfo->glidenParallelRsp = results.getBool( 9 );
		romInfo->overclockCPU = results.getBool( 10 );
		romInfo->overclockVI = results.getBool( 11 );
		romInfo->widescreen = (WidescreenMode)results.getInt( 12 );
		romInfo->bigEEPROM = results.getBool( 13 );
		romInfo->useInterpreter = results.getBool( 14 );
		romInfo->inputModeId = results.getUuid( 15 );
		romInfo->lastPlayed = results.getLong( 16 );
		romInfo->playTime = results.getLong( 17 );
		romInfo->crc32 = results.getUInt( 18 );
		romInfo->inputTypes = results.getInputTypes( 19 );
		romInfo->sdCard = results.getString( 20 );
		romInfo->groups = std::set<string>();

		if( includeGroups && !results.isNull( 21 ) ) {
			addGroups( romInfo->groups, results.getString( 21 ) );
		}
	}

	return true;
}

bool DataProvider::tryFetchRomByPath( const fs::path &filePath, bool includeGroups, RomInfo *romInfo, RomFile *romFile ) {
	static SqlCommand s_cmdWithoutGroups( R"#(
SELECT sha1, name, internal_name, emulator, parallel_plugin, mupen_plugin, upscale_texrects, remove_borders, emulate_framebuffer, n64_depth_compare, gliden_lle_rsp, overclock_cpu, overclock_vi, widescreen, big_eeprom, use_interpreter, input_mode_id, last_played, play_time, crc32, input_types, sd_card, last_modified, local
FROM ROMS INNER JOIN ROM_PATHS USING( sha1 )
WHERE path = ?1
)#"
	);

	static SqlCommand s_cmdWithGroups( R"#(
SELECT sha1, name, internal_name, emulator, parallel_plugin, mupen_plugin, upscale_texrects, remove_borders, emulate_framebuffer, n64_depth_compare, gliden_lle_rsp, overclock_cpu, overclock_vi, widescreen, big_eeprom, use_interpreter, input_mode_id, last_played, play_time, crc32, input_types, sd_card, last_modified, local, groups
FROM TAGGED_ROMS INNER JOIN ROM_PATHS USING( sha1 )
WHERE path = ?1
)#"
	);

	SqlCommand &cmd = (includeGroups && romInfo != nullptr) ? s_cmdWithGroups : s_cmdWithoutGroups;
	cmd.addParameter( filePath.u8string(), DATA_COPY );

	DataRecordIterator results = cmd.execQuery();
	if( !results.moveNext() ) return false;

	if( romInfo != nullptr ) {
		romInfo->sha1 = results.getString( 0 );
		romInfo->name = results.getString( 1 );
		romInfo->internalName = results.getString( 2 );
		romInfo->emulator = (EmulatorCore)results.getInt( 3 );
		romInfo->parallelPlugin = (GfxPlugin)results.getInt( 4 );
		romInfo->mupenPlugin = (GfxPlugin)results.getInt( 5 );
		romInfo->parallelTexRectUpscaling = results.getBool( 6 );
		romInfo->parallelRemoveBorders = results.getBool( 7 );
		romInfo->glidenFramebufferEmulation = results.getBool( 8 );
		romInfo->glidenCorrectDepthCompare = results.getBool( 9 );
		romInfo->glidenParallelRsp = results.getBool( 10 );
		romInfo->overclockCPU = results.getBool( 11 );
		romInfo->overclockVI = results.getBool( 12 );
		romInfo->widescreen = (WidescreenMode)results.getInt( 13 );
		romInfo->bigEEPROM = results.getBool( 14 );
		romInfo->useInterpreter = results.getBool( 15 );
		romInfo->inputModeId = results.getUuid( 16 );
		romInfo->lastPlayed = results.getLong( 17 );
		romInfo->playTime = results.getLong( 18 );
		romInfo->crc32 = results.getUInt( 19 );
		romInfo->inputTypes = results.getInputTypes( 20 );
		romInfo->sdCard = results.getString( 21 );
		romInfo->groups = std::set<string>();

		if( includeGroups && !results.isNull( 24 ) ) {
			addGroups( romInfo->groups, results.getString( 24 ) );
		}
	}

	if( romFile != nullptr ) {
		romFile->path = filePath;
		romFile->lastModified = results.getLong( 22 );
		romFile->sha1 = results.getString( 0 );
		romFile->local = results.getBool( 23 );
	}

	return true;
}

bool DataProvider::tryFetchRomFile( const fs::path &filePath, RomFile *romFile ) {
	static SqlCommand s_cmd( R"#(
SELECT last_modified, sha1, local
FROM ROM_PATHS INNER JOIN ROMS USING( sha1 )
WHERE path = ?1
)#"
	);

	s_cmd.addParameter( filePath.u8string(), DATA_COPY );
	DataRecordIterator results = s_cmd.execQuery();
	if( results.moveNext() ) {
		if( romFile != nullptr ) {
			romFile->path = filePath;
			romFile->lastModified = results.getLong( 0 );
			romFile->sha1 = results.getString( 1 );
			romFile->local = results.getBool( 2 );
		}
		return true;
	} else return false;
}

bool DataProvider::tryFetchRomPathWithCrc32( uint crc32, fs::path &filePath ) {
	static SqlCommand s_cmd( R"#(
SELECT path
FROM ROMS INNER JOIN ROM_PATHS USING( sha1 )
WHERE crc32 = ?1
)#"
	);

	s_cmd.addParameter( crc32 );

	DataRecordIterator results = s_cmd.execQuery();
	while( results.moveNext() ) {
		filePath = results.getPath( 0 );
		if( fs::existsSafe( filePath ) ) return true;
	}

	return false;
}

bool DataProvider::tryFetchRomPathWithSha1( const string &sha1, fs::path &filePath ) {
	static SqlCommand s_cmd( R"#(
SELECT path
FROM ROMS INNER JOIN ROM_PATHS USING( sha1 )
WHERE sha1 = ?1
AND local = 1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );

	DataRecordIterator results = s_cmd.execQuery();
	while( results.moveNext() ) {
		filePath = results.getPath( 0 );
		if( fs::existsSafe( filePath ) ) return true;
	}

	return false;
}

bool DataProvider::tryFetchRomFileWithSha1( const string &sha1, const string &playlistHint, RomFile *romFile ) {
	static SqlCommand s_cmd( R"#(
SELECT path, last_modified, sha1, local FROM ROMS
INNER JOIN ROM_PATHS USING( sha1 )
LEFT JOIN ROM_GROUPS USING( sha1 )
WHERE sha1 = ?1
ORDER BY
	CASE WHEN group_name = ?2 THEN 1
	ELSE 0
END DESC
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( playlistHint, DATA_REF );

	DataRecordIterator results = s_cmd.execQuery();
	while( results.moveNext() ) {
		romFile->path = results.getPath( 0 );
		romFile->lastModified = results.getLong( 1 );
		romFile->sha1 = results.getString( 2 );
		romFile->local = results.getBool( 3 );

		if( romFile->local && fs::existsSafe( romFile->path ) ) return true;
	}

	return false;
}

int DataProvider::countRomFilesWithSha1( const string &sha1 ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT COUNT(*)
FROM ROM_PATHS
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	DataRecordIterator results = s_cmd.execQuery();
	return results.moveNext() ? results.getInt( 0 ) : 0;
}

int DataProvider::countRomsInGroup( const string &groupName ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT COUNT(*)
FROM ROM_PATHS INNER JOIN ROM_GROUPS USING( sha1 )
WHERE group_name = ?1
)#"
	);

	s_cmd.addParameter( groupName, DATA_REF );
	DataRecordIterator results = s_cmd.execQuery();
	return results.moveNext() ? results.getInt( 0 ) : 0;
}

int DataProvider::countRomFiles() noexcept {
	static SqlCommand s_cmd( R"#(
SELECT COUNT(*)
FROM ROM_PATHS
WHERE local = 1
)#"
	);

	DataRecordIterator results = s_cmd.execQuery();
	return results.moveNext() ? results.getInt( 0 ) : 0;
}

bool DataProvider::romWithSha1Exists( const string &sha1 ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT 1
FROM ROMS INNER JOIN ROM_PATHS USING( sha1 )
WHERE sha1 = ?1
AND local = 1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	return s_cmd.execQuery().moveNext();
}

bool DataProvider::romWithPathExists( const fs::path &filePath ) {
	static SqlCommand s_cmd( R"#(
SELECT 1
FROM ROMS INNER JOIN ROM_PATHS USING( sha1 )
WHERE path = ?1
)#"
	);

	s_cmd.addParameter( filePath.u8string(), DATA_COPY );
	return s_cmd.execQuery().moveNext();
}

std::set<string> DataProvider::fetchPersistentGroups() {
	static SqlCommand s_cmd( R"#(
SELECT name FROM PERSISTENT_GROUPS
ORDER BY name ASC
)#"
	);

	std::set<string> results;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		results.insert( iterator.getString( 0 ) );
	}

	return results;
}

std::vector<string> DataProvider::fetchAllGroups() {
	static SqlCommand s_cmd( R"#(
SELECT DISTINCT group_name AS name
FROM ROM_GROUPS INNER JOIN ROM_PATHS USING( sha1 )
UNION
SELECT name FROM PERSISTENT_GROUPS
ORDER BY name ASC
)#"
	);

	std::vector<string> results;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		results.push_back( iterator.getString( 0 ) );
	}

	return results;
}

std::vector<RomFile> DataProvider::fetchAllRomFiles() {
	static SqlCommand s_cmd( R"#(
SELECT path, last_modified, sha1, local
FROM ROM_PATHS INNER JOIN ROMS USING( sha1 )
)#"
	);

	std::vector<RomFile> results;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		results.push_back({
			iterator.getPath( 0 ),
			iterator.getLong( 1 ),
			iterator.getString( 2 ),
			iterator.getBool( 3 )
		});
	}

	return results;
}

std::vector<RomFile> DataProvider::fetchLocalRomFiles() {
	static SqlCommand s_cmd( R"#(
SELECT path, last_modified, sha1
FROM ROM_PATHS INNER JOIN ROMS USING( sha1 )
WHERE local = 1
)#"
	);

	std::vector<RomFile> results;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		results.push_back({
			iterator.getPath( 0 ),
			iterator.getLong( 1 ),
			iterator.getString( 2 ),
			true
		});
	}

	return results;
}

HashMap<string,RomInfo> DataProvider::fetchAllRomInfo() {
	static SqlCommand s_cmd( R"#(
SELECT sha1, name, internal_name, emulator, parallel_plugin, mupen_plugin, upscale_texrects, remove_borders, emulate_framebuffer, n64_depth_compare, gliden_lle_rsp, overclock_cpu, overclock_vi, widescreen, big_eeprom, use_interpreter, input_mode_id, last_played, play_time, crc32, input_types, sd_card, groups
FROM TAGGED_ROMS
)#"
	);

	HashMap<string,RomInfo> results;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		string sha1 = iterator.getString( 0 );
		RomInfo &romInfo = results[sha1];
		romInfo.sha1 = std::move( sha1 );
		romInfo.name = iterator.getString( 1 );
		romInfo.internalName = iterator.getString( 2 );
		romInfo.emulator = (EmulatorCore)iterator.getInt( 3 );
		romInfo.parallelPlugin = (GfxPlugin)iterator.getInt( 4 );
		romInfo.mupenPlugin = (GfxPlugin)iterator.getInt( 5 );
		romInfo.parallelTexRectUpscaling = iterator.getBool( 6 );
		romInfo.parallelRemoveBorders = iterator.getBool( 7 );
		romInfo.glidenFramebufferEmulation = iterator.getBool( 8 );
		romInfo.glidenCorrectDepthCompare = iterator.getBool( 9 );
		romInfo.glidenParallelRsp = iterator.getBool( 10 );
		romInfo.overclockCPU = iterator.getBool( 11 );
		romInfo.overclockVI = iterator.getBool( 12 );
		romInfo.widescreen = (WidescreenMode)iterator.getInt( 13 );
		romInfo.bigEEPROM = iterator.getBool( 14 );
		romInfo.useInterpreter = iterator.getBool( 15 );
		romInfo.inputModeId = iterator.getUuid( 16 );
		romInfo.lastPlayed = iterator.getLong( 17 );
		romInfo.playTime = iterator.getLong( 18 );
		romInfo.crc32 = iterator.getUInt( 19 );
		romInfo.inputTypes = iterator.getInputTypes( 20 );
		romInfo.sdCard = iterator.getString( 21 );
		romInfo.groups = std::set<string>();
		if( !iterator.isNull( 22 ) ) {
			addGroups( romInfo.groups, iterator.getString( 22 ) );
		}
	}

	return results;
}

HashSet<string> DataProvider::fetchAllKnownChecksums() {
	static SqlCommand s_cmd( "SELECT DISTINCT sha1 FROM ROM_PATHS" );

	HashSet<string> checksums;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		checksums.insert( iterator.getString( 0 ) );
	}
	return checksums;
}

std::vector<fs::path> DataProvider::fetchManualRomPaths() {
	static SqlCommand s_cmd( R"#(
SELECT path
FROM MANUAL_ROM_PATHS
ORDER BY path ASC
)#" );

	std::vector<fs::path> results;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		results.push_back( iterator.getPath( 0 ) );
	}

	return results;
}

void DataProvider::addManualRomPath( const fs::path &path ) {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO MANUAL_ROM_PATHS( path )
VALUES( ?1 )
)#"
	);

	s_cmd.addParameter( path.u8string(), DATA_COPY );
	s_cmd.execNonQuery();
}

void DataProvider::deleteManualRomPath( const fs::path &path ) {
	static SqlCommand s_cmd( R"#(
DELETE FROM MANUAL_ROM_PATHS
WHERE path = ?1
)#"
	);

	s_cmd.addParameter( path.u8string(), DATA_COPY );
	s_cmd.execNonQuery();
}

void DataProvider::deleteAllManualRomPaths() noexcept {
	static SqlCommand s_cmd( "DELETE FROM MANUAL_ROM_PATHS" );
	s_cmd.execNonQuery();
}

void DataProvider::updateRomFile( const RomFile &romFile ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROM_PATHS SET
	last_modified = ?2,
	sha1 = ?3
WHERE path = ?1
)#"
	);

	s_cmd.addParameter( romFile.path.u8string(), DATA_COPY );
	s_cmd.addParameter( romFile.lastModified );
	s_cmd.addParameter( romFile.sha1, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::cloneRomInfo(
	const string &oldSha1,
	const string &newSha1,
	const string &internalName,
	uint crc32,
	const N64InputTypes &inputTypes
) noexcept {
	static SqlBatch s_cmd( R"#(
INSERT OR IGNORE INTO ROMS(
	sha1, name, internal_name, emulator, parallel_plugin, mupen_plugin, upscale_texrects, remove_borders, emulate_framebuffer, n64_depth_compare, gliden_lle_rsp, overclock_cpu, overclock_vi, widescreen, big_eeprom, use_interpreter, input_mode_id, last_played, play_time, crc32, input_types, sd_card
)
SELECT
	?2 AS sha1,
	name,
	?3 AS internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	upscale_texrects,
	remove_borders,
	emulate_framebuffer,
	n64_depth_compare,
	gliden_lle_rsp,
	overclock_cpu,
	overclock_vi,
	widescreen,
	big_eeprom,
	use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	?4 AS crc32,
	?5 AS input_types,
	sd_card
FROM ROMS
WHERE sha1 = ?1 
ON CONFLICT( sha1 ) DO UPDATE SET
	internal_name = excluded.internal_name,
	crc32 = excluded.crc32,
	last_played = max( ROMS.last_played, excluded.last_played ),
	play_time = max( ROMS.play_time, excluded.play_time );

INSERT OR IGNORE INTO ROM_GROUPS( sha1, group_name )
SELECT ?2 AS sha1, group_name
FROM ROM_GROUPS
WHERE sha1 = ?1;
)#"
	);

	s_cmd.addParameter( oldSha1, DATA_REF );
	s_cmd.addParameter( newSha1, DATA_REF );
	s_cmd.addParameter( internalName, DATA_REF );
	s_cmd.addParameter( crc32 );
	s_cmd.addParameter( inputTypes, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::deleteRomPath( const fs::path &filePath ) {
	static SqlCommand s_cmd( R"#(
DELETE FROM ROM_PATHS
WHERE path = ?1
)#"
	);

	s_cmd.addParameter( filePath.u8string(), DATA_COPY );
	s_cmd.execNonQuery();
}

void DataProvider::deleteRomInfo( const string &sha1 ) noexcept {
	static SqlBatch s_cmd( R"#(
DELETE FROM ROMS
WHERE sha1 = ?1;

DELETE FROM ROM_GROUPS
WHERE sha1 = ?1;
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::addToGroup( const string &sha1, const string &groupName ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO ROM_GROUPS( sha1, group_name )
VALUES( ?1, ?2 )
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( groupName, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::removeFromGroup( const string &sha1, const string &groupName ) noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM ROM_GROUPS
WHERE sha1 = ?1
AND group_name = ?2
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( groupName, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::setRomParallelTexRectUpscaling( const string &sha1, bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET upscale_texrects = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setRomParallelRemoveBorders( const string &sha1, bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET remove_borders = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setRomGlidenFrameBufferEmulation( const string &sha1, bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET emulate_framebuffer = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setRomGlidenAccurateDepthCompare( const string &sha1, bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET n64_depth_compare = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setRomGlidenRspEmulation( const string &sha1, bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET gliden_lle_rsp = ?2
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setDefaultParallelTexRectUpscaling( bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET upscale_texrects = ?1
	WHERE emulator == 0 
	OR (emulator == 1 AND parallel_plugin != 1)
	OR (emulator == 2 AND mupen_plugin != 1)
)#"
	);

	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setDefaultParallelRemoveBorders( bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET remove_borders = ?1
	WHERE emulator == 0 
	OR (emulator == 1 AND parallel_plugin != 1)
	OR (emulator == 2 AND mupen_plugin != 1)
)#"
	);

	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setDefaultGlidenFrameBufferEmulation( bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET emulate_framebuffer = ?1
	WHERE emulator != 2
	OR mupen_plugin != 2
)#"
	);

	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setDefaultGlidenAccurateDepthCompare( bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET n64_depth_compare = ?1
	WHERE emulator != 2
	OR mupen_plugin != 2
)#"
	);

	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setAllParallelTexRectUpscaling( bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET upscale_texrects = ?1
)#"
	);

	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setAllGlidenFrameBufferEmulation( bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET emulate_framebuffer = ?1
)#"
	);

	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

void DataProvider::setAllGlidenAccurateDepthCompare( bool enabled ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET n64_depth_compare = ?1
)#"
	);

	s_cmd.addParameter( enabled );
	s_cmd.execNonQuery();
}

string DataProvider::getLibplSdCard( const string &uid ) noexcept {
	static SqlCommand s_cmd( "SELECT name FROM LIBPL_SD_CARDS WHERE uid = ?1" );
	s_cmd.addParameter( uid, DATA_REF );
	
	DataRecordIterator results = s_cmd.execQuery();
	return results.moveNext() ? results.getString( 0 ) : string();
}

void DataProvider::registerLibplSdCard( const string &uid, const string &name ) noexcept {
	static SqlCommand s_cmd( "INSERT OR REPLACE INTO LIBPL_SD_CARDS( uid, name ) VALUES( ?1, ?2 )" );
	s_cmd.addParameter( uid, DATA_REF );
	s_cmd.addParameter( name, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::removeLibplSdCard( const string &uid ) noexcept {
	static SqlCommand s_cmd( "DELETE FROM LIBPL_SD_CARDS WHERE uid = ?1" );
	s_cmd.addParameter( uid, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::addOrUpdateRhdcHack( const RhdcHackInfo &hack ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR REPLACE INTO RHDC_HACKS( id, name, slug, description, star_count, downloads, avg_rating, avg_difficulty, category, submit_starpower )
VALUES( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10 )
)#"
	);

	s_cmd.addParameter( hack.hackId, DATA_REF );
	s_cmd.addParameter( hack.name, DATA_REF );
	s_cmd.addParameter( hack.slug, DATA_REF );
	s_cmd.addParameter( hack.description, DATA_REF );
	s_cmd.addParameter( hack.starCount );
	s_cmd.addParameter( hack.downloads );
	s_cmd.addParameter( hack.avgRating );
	s_cmd.addParameter( hack.avgDifficulty );
	s_cmd.addParameter( hack.category, DATA_REF );
	s_cmd.addParameter( hack.submitStarpower );
	s_cmd.execNonQuery();
}

void DataProvider::clearRhdcHackVersions( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM RHDC_HACK_VERSIONS
WHERE id = ?1
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::addRhdcHackVersion(
	const string &hackId,
	const string &sha1,
	const string &name,
	int sortOrder,
	GfxPlugin recommendedPlugin,
	RhdcHackFlag hackFlags,
	bool archived
) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR REPLACE INTO RHDC_HACK_VERSIONS( sha1, id, version_name, sort_order, recommended_plugin, hack_flags, archived )
VALUES( ?2, ?1, ?3, ?4, ?5, ?6, ?7 )
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.addParameter( name, DATA_REF );
	s_cmd.addParameter( sortOrder );
	s_cmd.addParameter( (int)recommendedPlugin );
	s_cmd.addParameter( (int)hackFlags );
	s_cmd.addParameter( archived );
	s_cmd.execNonQuery();
}

void DataProvider::addRhdcHackAuthor( const string &hackId, const string &author ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO RHDC_HACK_AUTHORS( id, author )
VALUES( ?1, ?2 )
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( author, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::clearRhdcHacks() noexcept {
	static SqlBatch s_cmd( R"#(
		DELETE FROM RHDC_HACKS;
		DELETE FROM RHDC_HACK_AUTHORS;
		DELETE FROM RHDC_HACK_VERSIONS;
		DELETE FROM ROM_GROUPS WHERE group_name = ?1;
		DELETE FROM ROM_GROUPS WHERE group_name = ?2;
		DELETE FROM ROM_GROUPS WHERE group_name = ?3;
		DELETE FROM ROM_PATHS WHERE local = 0;
)#"
	);

	s_cmd.addParameter( SpecialGroups::WantToPlay, DATA_REF );
	s_cmd.addParameter( SpecialGroups::InProgress, DATA_REF );
	s_cmd.addParameter( SpecialGroups::Completed, DATA_REF );

	s_cmd.execNonQuery();
}

void DataProvider::removeOnlineRom( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
		DELETE FROM ROM_PATHS
		WHERE sha1 IN (
			SELECT sha1 FROM RHDC_HACK_VERSIONS
			WHERE id = ?1
		)
		AND local = 0
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::updateRhdcProgress( const string &hackId, const RhdcHackProgress &progress ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR REPLACE INTO RHDC_HACK_PROGRESS( id, my_rating, my_difficulty, stars_collected, completed )
VALUES( ?1, ?2, ?3, ?4, ?5 )
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( (int)progress.rating );
	s_cmd.addParameter( progress.difficulty );
	s_cmd.addParameter( progress.stars );
	s_cmd.addParameter( progress.completed );
	s_cmd.execNonQuery();
}

void DataProvider::updateRhdcHackRating( const string &hackId, RhdcRating quality, ubyte difficulty ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE RHDC_HACK_PROGRESS SET
	my_rating = ?2,
	my_difficulty = ?3
WHERE id = ?1
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( (int)quality );
	s_cmd.addParameter( difficulty );
	s_cmd.execNonQuery();
}

void DataProvider::updateRhdcStarProgress( const string &hackId, ushort stars, bool completed ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE RHDC_HACK_PROGRESS SET
	stars_collected = ?2,
	completed = ?3
WHERE id = ?1
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( stars );
	s_cmd.addParameter( completed );
	s_cmd.execNonQuery();
}

void DataProvider::suggestRhdcHackPlayTime( const string &hackId, int64 playTime ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE ROMS
SET play_time = ?2
WHERE ROMS.sha1 IN( SELECT sha1 FROM RHDC_HACK_VERSIONS WHERE id = ?1 )
AND play_time < ?2
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( playTime );
	s_cmd.execNonQuery();
}

int64 DataProvider::getPlayTimeByHackId( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT MAX( play_time ) FROM ROMS
INNER JOIN RHDC_HACK_VERSIONS USING( sha1 )
WHERE id = ?1
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );

	DataRecordIterator iterator = s_cmd.execQuery();
	if( iterator.moveNext() && !iterator.isNull( 0 ) ) {
		return iterator.getLong( 0 );
	}

	return -1ll;
}

HashMap<string,BasicRhdcHack> DataProvider::fetchFollowedRhdcHacks() {
	static SqlCommand s_cmd( R"#(
SELECT id, name, star_count, category, my_rating, my_difficulty, stars_collected, completed, submit_starpower
FROM RHDC_HACKS LEFT JOIN RHDC_HACK_PROGRESS USING( id )
)#"
	);

	HashMap<string,BasicRhdcHack> hackMap;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		const string hackId = iterator.getString( 0 );
		hackMap[hackId] = BasicRhdcHack{
			BasicRhdcHackInfo {
				hackId,
				iterator.getString( 1 ),
				iterator.getInt( 2 ),
				iterator.getString( 3 ) == "Kaizo",
				iterator.getBool( 8 )
			},
			iterator.isNull( 4 ) ?
				RhdcHackProgress { RhdcRating::NotRated, 0, 0, false } :
				RhdcHackProgress {
					(RhdcRating)iterator.getUInt( 4 ),
					(ubyte)iterator.getInt( 5 ),
					(ushort)iterator.getInt( 6 ),
					iterator.getBool( 7 )
				}
		};
	}

	return hackMap;
}

HashMap<string,string> DataProvider::fetchAllRhdcHackVersions() {
	static SqlCommand s_cmd( R"#(
SELECT sha1, id
FROM RHDC_HACK_VERSIONS
)#"
	);

	HashMap<string,string> hackMap;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		hackMap[iterator.getString( 0 )] = iterator.getString( 1 );
	}

	return hackMap;
}

HashMap<string,StarpowerExt> DataProvider::fetchAllStarpower() {
	static SqlCommand s_cmd( R"#(
SELECT
	id,
	my_rating,
	my_difficulty,
	stars_collected,
	completed,
	MAX( play_time ),
	star_count
FROM ROMS
INNER JOIN RHDC_HACK_VERSIONS USING( sha1 )
INNER JOIN RHDC_HACKS USING( id )
INNER JOIN RHDC_HACK_PROGRESS USING( id )
GROUP BY( id )
)#"
	);

	HashMap<string,StarpowerExt> starpowerMap;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		starpowerMap[iterator.getString( 0 )] = StarpowerExt{
			RhdcHackProgress{
				(RhdcRating)iterator.getUInt( 1 ),
				(ubyte)iterator.getInt( 2 ),
				(ushort)iterator.getInt( 3 ),
				iterator.getBool( 4 )
			},
			iterator.getLong( 5 ),
			iterator.getInt( 6 )
		};
	}

	return starpowerMap;
}

bool DataProvider::tryFetchRhdcHackByChecksum( const string &sha1, RhdcHack *hack ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT
	id,
	name,
	slug,
	description,
	star_count,
	downloads,
	avg_rating,
	avg_difficulty,
	category,
	my_rating,
	my_difficulty,
	stars_collected,
	completed,
	submit_starpower
FROM RHDC_HACK_VERSIONS
INNER JOIN RHDC_HACKS USING( id )
LEFT JOIN RHDC_HACK_PROGRESS USING( id )
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );
	DataRecordIterator iterator = s_cmd.execQuery();
	if( !iterator.moveNext() ) {
		return false;
	}

	hack->info = RhdcHackInfo{
		iterator.getString( 0 ),
		iterator.getString( 1 ),
		iterator.getString( 2 ),
		iterator.getString( 3 ),
		iterator.getInt( 4 ),
		iterator.getInt( 5 ),
		iterator.getFloat( 6 ),
		iterator.getFloat( 7 ),
		iterator.getString( 8 ),
		iterator.getBool( 13 )
	};

	if( iterator.isNull( 9 ) ) {
		hack->progress = RhdcHackProgress{ RhdcRating::NotRated, 0, 0, false };
	} else {
		hack->progress = RhdcHackProgress{
			(RhdcRating)iterator.getUInt( 9 ),
			(ubyte)iterator.getInt( 10 ),
			(ushort)iterator.getInt( 11 ),
			iterator.getBool( 12 )
		};
	}

	return true;
}

bool DataProvider::tryFetchHackIdByChecksum( const string &sha1, string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT id FROM RHDC_HACK_VERSIONS
WHERE sha1 = ?1
)#"
	);
	s_cmd.addParameter( sha1, DATA_REF );

	DataRecordIterator iterator = s_cmd.execQuery();
	if( iterator.moveNext() ) {
		hackId.assign( iterator.getString( 0 ) );
		return true;
	}

	hackId.clear();
	return false;
}

std::set<string> DataProvider::fetchGroupsForRhdcHack( const string &hackId ) {
	static SqlCommand s_cmd( R"#(
SELECT DISTINCT group_name
FROM ROM_GROUPS
INNER JOIN RHDC_HACK_VERSIONS USING( sha1 )
INNER JOIN ROM_PATHS USING( sha1 )
WHERE id = ?1
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );

	std::set<string> groups;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		groups.insert( iterator.getString( 0 ) );
	}
	return groups;
}

void DataProvider::addRhdcHackToGroup( const string &hackId, const string &groupName ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO ROM_GROUPS( sha1, group_name )
SELECT sha1, ?2 AS group_name
FROM RHDC_HACK_VERSIONS INNER JOIN ROMS USING( sha1 ) 
WHERE id = ?1
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( groupName, DATA_REF );
	s_cmd.execNonQuery();
}

void DataProvider::removeRhdcHackFromGroup( const string &hackId, const string &groupName ) noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM ROM_GROUPS
WHERE sha1 IN (
	SELECT sha1 FROM RHDC_HACK_VERSIONS
	WHERE id = ?1
)
AND group_name = ?2
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( groupName, DATA_REF );
	s_cmd.execNonQuery();
}

bool DataProvider::hasRhdcData() noexcept {
	static SqlCommand s_cmd( R"#(
SELECT id FROM RHDC_HACK_PROGRESS
UNION
SELECT id FROM RHDC_HACK_VERSIONS
LIMIT 1
)#"
	);

	DataRecordIterator iterator = s_cmd.execQuery();
	return iterator.moveNext();
}

void DataProvider::clearAllRhdcData() noexcept {
	static SqlBatch s_cmd( R"#(
DELETE FROM RHDC_HACKS;
DELETE FROM RHDC_HACK_PROGRESS;
DELETE FROM RHDC_HACK_VERSIONS;
DELETE FROM RHDC_HACK_AUTHORS;

DELETE FROM ROM_PATHS WHERE local = 0;
)#"
	);

	s_cmd.execNonQuery();
}

void DataProvider::forgetHack( const string &hackId ) noexcept {
	static SqlBatch s_cmd( R"#(
DELETE FROM ROM_PATHS
WHERE local = 0
AND sha1 IN (
	SELECT sha1 FROM RHDC_HACK_VERSIONS
	WHERE id = ?1
);

DELETE FROM RHDC_HACK_AUTHORS
WHERE id = ?1;

DELETE FROM RHDC_HACK_PROGRESS
WHERE id = ?1;

DELETE FROM RHDC_HACK_VERSIONS
WHERE id = ?1;

DELETE FROM RHDC_HACKS
WHERE id = ?1;
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.execNonQuery();
}

static std::vector<RhdcHackExt> readExtRhdcHacks( DataRecordIterator &&iterator ) {
	std::vector<RhdcHackExt> results;
	while( iterator.moveNext() ) {
		std::vector<string> authors;
		RhdcHackProgress progress = { RhdcRating::NotRated, 0, 0, false };

		if( !iterator.isNull( 10 ) ) {
			const string authorsString = iterator.getString( 10 );

			size_t nameStart = 0;
			for( size_t i = 0; i < authorsString.length(); i++ ) {
				if( authorsString[i] != '\n' ) continue;
				if( i != nameStart ) {
					authors.push_back( authorsString.substr( nameStart, i - nameStart ) );
				}
				nameStart = i + 1;
			}

			if( nameStart < authorsString.length() ) {
				authors.push_back( authorsString.substr( nameStart ) );
			}
		}

		if( !iterator.isNull( 11 ) ) {
			progress.rating = (RhdcRating)iterator.getUInt( 11 );
			progress.difficulty = (ubyte)iterator.getUInt( 12 );
			progress.stars = (ushort)iterator.getUInt( 13 );
			progress.completed = iterator.getBool( 14 );
		}

		results.push_back({
			RhdcHackInfo {
				iterator.getString( 0 ),
				iterator.getString( 1 ),
				iterator.getString( 2 ),
				iterator.getString( 3 ),
				iterator.getInt( 4 ),
				iterator.getInt( 5 ),
				iterator.getFloat( 6 ),
				iterator.getFloat( 7 ),
				iterator.getString( 8 ),
				iterator.getBool( 9 )
			},
			std::move( progress ),
			std::move( authors ),
			iterator.getLong( 15 ),
			iterator.getLong( 16 )
		});
	}
	return results;
}

std::vector<RhdcHackExt> DataProvider::fetchExtRhdcHacks( const string &searchText ) {
	static SqlCommand s_cmd( R"#(
SELECT
	id, name, slug, description, star_count, downloads, avg_rating, avg_difficulty, category, submit_starpower,
	group_concat( author, x'0A' ) AS authors,
	my_rating, my_difficulty, stars_collected, completed,
	last_played, play_time
FROM (
	SELECT
		RHDC_HACKS.*,
		RHDC_HACK_PROGRESS.*,
		max( ROMS.last_played ) AS last_played,
		max( ROMS.play_time ) AS play_time
	FROM RHDC_HACKS
	LEFT JOIN RHDC_HACK_PROGRESS USING( id )
	INNER JOIN RHDC_HACK_VERSIONS USING( id )
	INNER JOIN ROMS USING( sha1 )
		WHERE RHDC_HACKS.name LIKE ?1 ESCAPE '\'
	GROUP BY id
)
LEFT JOIN RHDC_HACK_AUTHORS USING( id )
GROUP BY id
)#"
	);

	s_cmd.addParameter( escapePattern( searchText ), DATA_COPY );
	return readExtRhdcHacks( s_cmd.execQuery() );
}

std::vector<RhdcHackExt> DataProvider::fetchExtRhdcHacksInList( const string &playlist, const string &searchText ) {
	static SqlCommand s_cmd( R"#(
SELECT
	id, name, slug, description, star_count, downloads, avg_rating, avg_difficulty, category, submit_starpower,
	group_concat( author, x'0A' ) AS authors,
	my_rating, my_difficulty, stars_collected, completed,
	last_played, play_time
FROM (
	SELECT
		RHDC_HACKS.*,
		RHDC_HACK_PROGRESS.*,
		max( ROMS.last_played ) AS last_played,
		max( ROMS.play_time ) AS play_time
	FROM RHDC_HACKS
	LEFT JOIN RHDC_HACK_PROGRESS USING( id )
	INNER JOIN RHDC_HACK_VERSIONS USING( id )
	INNER JOIN ROMS USING( sha1 )
	INNER JOIN ROM_GROUPS USING( sha1 )
		WHERE group_name = ?1
		AND RHDC_HACKS.name LIKE ?2 ESCAPE '\'
	GROUP BY id
)
LEFT JOIN RHDC_HACK_AUTHORS USING( id )
GROUP BY id
)#"
	);

	s_cmd.addParameter( playlist, DATA_REF );
	s_cmd.addParameter( escapePattern( searchText ), DATA_COPY );
	return readExtRhdcHacks( s_cmd.execQuery() );
}

std::vector<RhdcHackVersion> DataProvider::fetchRhdcHackVersions( const string &hackId ) {
	static SqlCommand s_cmd( R"#(
SELECT sha1, version_name, recommended_plugin, hack_flags, archived
FROM RHDC_HACK_VERSIONS
	WHERE id = ?1
	ORDER BY sort_order ASC
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );

	std::vector<RhdcHackVersion> versions;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		versions.push_back({
			iterator.getString( 0 ),
			iterator.getString( 1 ),
			(GfxPlugin)iterator.getInt( 2 ),
			(RhdcHackFlag)iterator.getInt( 3 ),
			iterator.getBool( 4 )
		});
	}

	return versions;
}

std::vector<fs::path> DataProvider::fetchRomPathsForRhdcHack( const string &hackId ) {
	static SqlCommand s_cmd( R"#(
SELECT path FROM RHDC_HACK_VERSIONS
INNER JOIN ROM_PATHS USING( sha1 )
WHERE id = ?1
)#"
	);
	s_cmd.addParameter( hackId, DATA_REF );

	std::vector<fs::path> paths;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		paths.push_back( iterator.getPath( 0 ) );
	}

	return paths;
}

std::vector<string> DataProvider::fetchRhdcPlaylists() {
	static SqlCommand s_cmd( R"#(
SELECT DISTINCT group_name FROM RHDC_HACKS
INNER JOIN RHDC_HACK_VERSIONS USING( id )
INNER JOIN ROM_GROUPS USING( sha1 )
INNER JOIN ROM_PATHS USING( sha1 )
ORDER BY group_name ASC
)#"
	);

	std::vector<string> playlists;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		playlists.push_back( iterator.getString( 0 ) );
	}
	return playlists;
}

string DataProvider::getLastPlayedVersion( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT sha1 FROM RHDC_LAST_PLAYED_VERSIONS
WHERE id = ?1
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	DataRecordIterator result = s_cmd.execQuery();
	return result.moveNext() ? result.getString( 0 ) : "";
}

void DataProvider::setLastPlayedVersion( const string &hackId, const string &sha1 ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR REPLACE INTO RHDC_LAST_PLAYED_VERSIONS( id, sha1 )
VALUES( ?1, ?2 )
)#"
	);

	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( sha1, DATA_REF );
	s_cmd.execNonQuery();
}

static inline void deleteOldLogs() noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM LOGS
WHERE ROWID IN (
	SELECT ROWID FROM LOGS
	ORDER BY ROWID DESC
	LIMIT -1 OFFSET 100
)
)#"
	);

	s_cmd.execNonQuery();
}

void DataProvider::addLogEntry( const LogEntry &logEntry ) noexcept {
	static int cleanupCounter = -1;
	static SqlCommand s_cmd( R"#(
INSERT INTO LOGS( level, timestamp, message )
VALUES( ?1, ?2, ?3 )
)#"
	);

	s_cmd.addParameter( (int)logEntry.level );
	s_cmd.addParameter( logEntry.timestamp );
	s_cmd.addParameter( logEntry.message, DATA_REF );
	s_cmd.execNonQuery();

	if( ++cleanupCounter % 10 == 0 ) {
		cleanupCounter = 0;
		deleteOldLogs();
	}
}

std::vector<LogEntry> DataProvider::fetchLogs() {
	static SqlCommand s_cmd( R"#(
SELECT level, timestamp, message
FROM LOGS
ORDER BY ROWID DESC
)#"
	);

	std::vector<LogEntry> logs;
	logs.reserve( 109 );

	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		logs.push_back({
			(LogLevel)iterator.getInt( 0 ),
			iterator.getLong( 1 ),
			iterator.getString( 2 )
		});
	}

	return logs;
}

RhdcSettingHints DataProvider::getRhdcHints( const string &sha1 ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT recommended_plugin, hack_flags
FROM RHDC_HACK_VERSIONS
WHERE sha1 = ?1
)#"
	);

	s_cmd.addParameter( sha1, DATA_REF );

	DataRecordIterator iterator = s_cmd.execQuery();
	if( iterator.moveNext() ) {
		return {
			(GfxPlugin)iterator.getInt( 0 ),
			(RhdcHackFlag)iterator.getInt( 1 )
		};
	}

	return { GfxPlugin::UseDefault, RhdcHackFlag::None };
}

string DataProvider::getVersionNameFromRomPath( const fs::path &romPath ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT version_name
FROM ROM_PATHS
INNER JOIN RHDC_HACK_VERSIONS USING( sha1 )
WHERE path = ?1
)#"
	);
	s_cmd.addParameter( romPath.u8string(), DATA_COPY );

	DataRecordIterator iterator = s_cmd.execQuery();
	return iterator.moveNext() ? iterator.getString( 0 ) : ""s;
}

std::vector<string> DataProvider::getPendingPluginChecks() noexcept {
	static SqlCommand s_cleanup( R"#(
DELETE FROM PENDING_PLUGIN_CHECKS
WHERE sha1 IN (
	SELECT sha1 FROM RHDC_HACK_VERSIONS
)
)#"
	);
	s_cleanup.execNonQuery();

	static SqlCommand s_cmd( R"#(
SELECT sha1 FROM PENDING_PLUGIN_CHECKS
ORDER BY sha1 ASC
LIMIT 1024
)#"
	);

	std::vector<string> checksums;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) {
		checksums.push_back( iterator.getString( 0 ) );
	}

	return checksums;
}

void DataProvider::addPendingPluginCheck( const string &sha1, bool force ) noexcept {
	if( !force ) {
		static SqlCommand s_test( R"#(
SELECT 1 FROM ROMS
WHERE sha1 = ?1
AND parallel_plugin <> 0
)#"
		);
		s_test.addParameter( sha1, DATA_REF );

		DataRecordIterator iterator = s_test.execQuery();
		if( iterator.moveNext() ) return;
	}

	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO PENDING_PLUGIN_CHECKS( sha1 )
VALUES( ?1 )
)#"
	);
	s_cmd.addParameter( sha1, DATA_REF );

	s_cmd.execNonQuery();
}

bool DataProvider::hasPendingPluginCheck( const string &sha1 ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT 1 FROM PENDING_PLUGIN_CHECKS
WHERE sha1 = ?1
)#"
	);
	s_cmd.addParameter( sha1, DATA_REF );

	DataRecordIterator iterator = s_cmd.execQuery();
	return iterator.moveNext();
}

void DataProvider::removePendingPluginCheck( const string &sha1 ) noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM PENDING_PLUGIN_CHECKS
WHERE sha1 = ?1
)#"
	);
	s_cmd.addParameter( sha1, DATA_REF );

	s_cmd.execNonQuery();
}

void DataProvider::removePendingPluginChecks() noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM PENDING_PLUGIN_CHECKS
WHERE sha1 IN (
	SELECT sha1 FROM PENDING_PLUGIN_CHECKS
	ORDER BY sha1 ASC
	LIMIT 1024
)
)#"
	);
	s_cmd.execNonQuery();
}

bool DataProvider::groupSaveEnabled( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT 1 FROM DISABLED_GROUP_SAVES
WHERE hack_id = ?1
)#"
	);
	s_cmd.addParameter( hackId, DATA_REF );

	DataRecordIterator iterator = s_cmd.execQuery();
	return !iterator.moveNext();
}

void DataProvider::enableGroupSave( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM DISABLED_GROUP_SAVES
WHERE hack_id = ?1
)#"
	);
	s_cmd.addParameter( hackId, DATA_REF );

	s_cmd.execNonQuery();
}

void DataProvider::disableGroupSave( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO DISABLED_GROUP_SAVES( hack_id )
VALUES( ?1 )
)#"
	);
	s_cmd.addParameter( hackId, DATA_REF );

	s_cmd.execNonQuery();
}

bool DataProvider::tryGetSaveGroup( const string &sha1, string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT hack_id FROM GROUP_SAVES
WHERE sha1 = ?1
)#"
	);
	s_cmd.addParameter( sha1, DATA_REF );

	DataRecordIterator iterator = s_cmd.execQuery();
	if( iterator.moveNext() ) {
		hackId.assign( iterator.getString( 0 ) );
		return true;
	}

	hackId.clear();
	return false;
}

bool DataProvider::tryGetSaveGroupForRomPath( const fs::path &romPath, string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT hack_id FROM ROM_PATHS
INNER JOIN GROUP_SAVES USING( sha1 )
WHERE path = ?1
)#"
	);
	s_cmd.addParameter( romPath.u8string(), DATA_COPY );

	DataRecordIterator iterator = s_cmd.execQuery();
	if( iterator.moveNext() ) {
		hackId.assign( iterator.getString( 0 ) );
		return true;
	}

	hackId.clear();
	return false;
}

bool DataProvider::isSaveGroupEmpty( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
SELECT sha1 FROM GROUP_SAVES
WHERE hack_id = ?1
LIMIT 1
)#"
	);
	s_cmd.addParameter( hackId, DATA_REF );

	DataRecordIterator iterator = s_cmd.execQuery();
	return !iterator.moveNext();
}

void DataProvider::addAllVersionsToSaveGroup( const string &hackId ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO GROUP_SAVES( hack_id, sha1 )
SELECT id AS hack_id, sha1 FROM RHDC_HACK_VERSIONS
WHERE id = ?1
)#"
	);
	s_cmd.addParameter( hackId, DATA_REF );

	s_cmd.execNonQuery();
}

void DataProvider::removeFromSaveGroup( const string &hackId, const string &sha1 ) noexcept {
	static SqlCommand s_cmd( R"#(
DELETE FROM GROUP_SAVES
WHERE hack_id = ?1
AND sha1 = ?2
)#"
	);
	s_cmd.addParameter( hackId, DATA_REF );
	s_cmd.addParameter( sha1, DATA_REF );

	s_cmd.execNonQuery();
}

void DataProvider::addCrashLog( int64 timestamp, const string &report ) noexcept {
	static SqlCommand s_cmd( R"#(
INSERT OR IGNORE INTO CRASH_LOGS( crash_date, report )
VALUES( ?1, ?2 )
)#"
	);
	s_cmd.addParameter( timestamp );
	s_cmd.addParameter( report, DATA_REF );

	s_cmd.execNonQuery();
}

void DataProvider::setCrashLogSentDate( int64 crashedAt, int64 sentAt ) noexcept {
	static SqlCommand s_cmd( R"#(
UPDATE CRASH_LOGS
SET sent_at = ?2
WHERE crash_date = ?1
)#"
	);
	s_cmd.addParameter( crashedAt );
	s_cmd.addParameter( sentAt );

	s_cmd.execNonQuery();
}

std::map<int64,string> DataProvider::getUnreportedCrashLogs() {
	static SqlCommand s_cmd( R"#(
SELECT crash_date, report FROM CRASH_LOGS
WHERE sent_at IS NULL
)#"
	);

	std::map<int64, string> reports;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) reports[iterator.getLong( 0 )] = iterator.getString( 1 );
	return reports;
}

std::map<int64,string> DataProvider::getCrashLogsForVersion( const Version &appVersion ) {
	static SqlCommand s_cmd( R"#(
SELECT crash_date, report FROM CRASH_LOGS
WHERE report LIKE ?1
AND report NOT LIKE '%[Warning]%'
)#"
	);
	s_cmd.addParameter( "%AppVersion = "s + appVersion.toString() + '%', DATA_COPY );

	std::map<int64, string> reports;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) reports[iterator.getLong( 0 )] = iterator.getString( 1 );
	return reports;
}

std::map<int64,string> DataProvider::getAllCrashLogs() {
	static SqlCommand s_cmd( R"#(
SELECT crash_date, report FROM CRASH_LOGS
WHERE report NOT LIKE '%[Warning]%'
)#"
	);

	std::map<int64, string> reports;
	DataRecordIterator iterator = s_cmd.execQuery();
	while( iterator.moveNext() ) reports[iterator.getLong( 0 )] = iterator.getString( 1 );
	return reports;
}

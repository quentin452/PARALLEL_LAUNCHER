#include "src/core/migration.hpp"

#include "src/core/file-controller.hpp"
#include "src/db/sqlite.hpp"
#include "src/db/transaction.hpp"

// Migration 01: added rom-specific options for ParaLLEl and GLideN64
inline static void migration_01() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('ROMS')
WHERE name = 'upscale_texrects'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlTransaction transaction;

  SqlBatch(R"#(
DROP VIEW TAGGED_ROMS;
DROP INDEX ix_rom_by_crc;

CREATE TABLE ROMS_v2(
	sha1 TEXT NOT NULL PRIMARY KEY,
	name TEXT NOT NULL,
	internal_name TEXT NOT NULL,
	emulator INTEGER NOT NULL,
	parallel_plugin INTEGER NOT NULL,
	mupen_plugin INTEGER NOT NULL,
	upscale_texrects INTEGER NOT NULL,
	emulate_framebuffer INTEGER NOT NULL,
	n64_depth_compare INTEGER NOT NULL,
	overclock_cpu INTEGER NOT NULL,
	overclock_vi INTEGER NOT NULL,
	widescreen INTEGER NOT NULL,
	input_mode_id BLOB NOT NULL,
	last_played INTEGER NOT NULL,
	play_time INTEGER NOT NULL,
	crc32 INTEGER NOT NULL
);
)#")
      .execNonQuery();

  SqlCommand migration(R"#(
INSERT INTO ROMS_v2(
	sha1,
	name,
	internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	upscale_texrects,
	emulate_framebuffer,
	n64_depth_compare,
	overclock_cpu,
	overclock_vi,
	widescreen,
	input_mode_id,
	last_played,
	play_time,
	crc32
)
SELECT
	sha1,
	name,
	internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	?1 AS upscale_texrects,
	?2 AS emulate_framebuffer,
	?3 AS n64_depth_compare,
	overclock_cpu,
	overclock_vi,
	widescreen,
	input_mode_id,
	last_played,
	play_time,
	crc32
FROM ROMS
)#");

  const AppSettings &settings = FileController::loadAppSettings();
  migration.addParameter(settings.parallelTexRectUpscaling);
  migration.addParameter(settings.glidenFramebufferEmulation);
  migration.addParameter(settings.glidenCorrectDepthCompare);
  migration.execNonQuery();

  SqlCommand("DROP TABLE ROMS").execNonQuery();

  SqlCommand("ALTER TABLE ROMS_v2 RENAME TO ROMS").execNonQuery();
  SqlCommand("CREATE INDEX ix_rom_by_crc ON ROMS( crc32 )").execNonQuery();

  SqlCommand(R"#(
CREATE VIEW TAGGED_ROMS AS
SELECT ROMS.*, group_concat( group_name, x'0A' ) AS groups
FROM ROMS LEFT JOIN ROM_GROUPS USING( sha1 )
GROUP BY sha1
)#")
      .execNonQuery();

  transaction.commit();
}

// Migration 02: Added local column to ROM_PATHS
inline static void migration_02() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('ROM_PATHS')
WHERE name = 'local'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand(R"#(
CREATE TABLE IF NOT EXISTS ROM_PATHS_v2(
	path TEXT NOT NULL PRIMARY KEY,
	last_modified INTEGER NOT NULL,
	sha1 TEXT NOT NULL,
	local INTEGER NOT NULL
)
)#")
      .execNonQuery();

  SqlCommand(R"#(
INSERT INTO ROM_PATHS_v2( path, last_modified, sha1, local )
SELECT path, last_modified, sha1, 1 AS local FROM ROM_PATHS;
)#")
      .execNonQuery();

  SqlCommand("DROP INDEX IF EXISTS ix_paths_by_sha1").execNonQuery();
  SqlCommand("DROP TABLE ROM_PATHS").execNonQuery();
  SqlCommand("ALTER TABLE ROM_PATHS_v2 RENAME TO ROM_PATHS").execNonQuery();
  SqlCommand("CREATE INDEX ix_paths_by_sha1 ON ROM_PATHS( sha1 )")
      .execNonQuery();
}

// Migration 03: Added version_name and sort_orer to RHDC_HACK_VERSIONS
inline static void migration_03() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('RHDC_HACK_VERSIONS')
WHERE name = 'version_name'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand(R"#(
CREATE TABLE IF NOT EXISTS RHDC_HACK_VERSIONS_v2(
	sha1 TEXT NOT NULL PRIMARY KEY,
	id TEXT NOT NULL,
	version_name TEXT NOT NULL,
	sort_order INTEGER NOT NULL
)
)#")
      .execNonQuery();

  SqlCommand(R"#(
INSERT INTO RHDC_HACK_VERSIONS_v2( sha1, id, version_name, sort_order )
SELECT sha1, id, 'Unknown' AS version_name, 0 AS sort_order FROM RHDC_HACK_VERSIONS
)#")
      .execNonQuery();

  SqlCommand("DROP INDEX IF EXISTS ix_hack_versions_by_id").execNonQuery();
  SqlCommand("DROP TABLE RHDC_HACK_VERSIONS").execNonQuery();
  SqlCommand("ALTER TABLE RHDC_HACK_VERSIONS_v2 RENAME TO RHDC_HACK_VERSIONS")
      .execNonQuery();
  SqlCommand("CREATE INDEX IF NOT EXISTS ix_hack_versions_by_id ON "
             "RHDC_HACK_VERSIONS( id )")
      .execNonQuery();
}

// Migration 04: For beta testers- Fix primary key in RHDC_LAST_PLAYED_VERSIONS
// table
inline static void migration_04() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('RHDC_LAST_PLAYED_VERSIONS')
WHERE name = 'sha1'
AND pk > 0
)#");

  if (!checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand("DROP TABLE RHDC_LAST_PLAYED_VERSIONS").execNonQuery();
  SqlCommand(R"#(
CREATE TABLE RHDC_LAST_PLAYED_VERSIONS(
	id TEXT NOT NULL PRIMARY KEY,
	sha1 TEXT NOT NULL
)
)#")
      .execNonQuery();
}

// Migration 05: Add submit_starpower column to RHDC_HACKS table
inline static void migration_05() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('RHDC_HACKS')
WHERE name = 'submit_starpower'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand(R"#(
CREATE TABLE IF NOT EXISTS RHDC_HACKS_v2(
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
)
)#")
      .execNonQuery();

  SqlCommand(R"#(
INSERT INTO RHDC_HACKS_v2(
	id,
	name,
	slug,
	description,
	star_count,
	downloads,
	avg_rating,
	avg_difficulty,
	category,
	submit_starpower
)
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
	1 AS submit_starpower
FROM RHDC_HACKS
)#")
      .execNonQuery();

  SqlCommand("DROP TABLE RHDC_HACKS").execNonQuery();
  SqlCommand("ALTER TABLE RHDC_HACKS_v2 RENAME TO RHDC_HACKS").execNonQuery();
}

// Migration 06: Added recommended_plugin column to RHDC_HACK_VERSIONS
inline static void migration_06() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('RHDC_HACK_VERSIONS')
WHERE name = 'recommended_plugin'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand(R"#(
ALTER TABLE RHDC_HACK_VERSIONS
ADD COLUMN recommended_plugin INTEGER NOT NULL DEFAULT 0
)#")
      .execNonQuery();
}

// Migration 07: Added hack_flags column to RHDC_HACK_VERSIONS
inline static void migration_07() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('RHDC_HACK_VERSIONS')
WHERE name = 'hack_flags'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand(R"#(
ALTER TABLE RHDC_HACK_VERSIONS
ADD COLUMN hack_flags INTEGER NOT NULL DEFAULT 0
)#")
      .execNonQuery();
}

// Migration 08: Added big_eeprom and use_interpreter columns to ROMS
inline static void migration_08() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('ROMS')
WHERE name = 'use_interpreter'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlTransaction transaction;

  SqlBatch(R"#(
DROP VIEW TAGGED_ROMS;
DROP INDEX ix_rom_by_crc;

CREATE TABLE ROMS_v2(
	sha1 TEXT NOT NULL PRIMARY KEY,
	name TEXT NOT NULL,
	internal_name TEXT NOT NULL,
	emulator INTEGER NOT NULL,
	parallel_plugin INTEGER NOT NULL,
	mupen_plugin INTEGER NOT NULL,
	upscale_texrects INTEGER NOT NULL,
	emulate_framebuffer INTEGER NOT NULL,
	n64_depth_compare INTEGER NOT NULL,
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
)#")
      .execNonQuery();

  SqlCommand(R"#(
INSERT INTO ROMS_v2(
	sha1,
	name,
	internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	upscale_texrects,
	emulate_framebuffer,
	n64_depth_compare,
	overclock_cpu,
	overclock_vi,
	widescreen,
	big_eeprom,
	use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	crc32
)
SELECT
	sha1,
	name,
	internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	upscale_texrects,
	emulate_framebuffer,
	n64_depth_compare,
	overclock_cpu,
	overclock_vi,
	widescreen,
	0 AS big_eeprom,
	0 AS use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	crc32
FROM ROMS
)#")
      .execNonQuery();

  SqlCommand("DROP TABLE ROMS").execNonQuery();

  SqlCommand("ALTER TABLE ROMS_v2 RENAME TO ROMS").execNonQuery();
  SqlCommand("CREATE INDEX ix_rom_by_crc ON ROMS( crc32 )").execNonQuery();

  SqlCommand(R"#(
CREATE VIEW TAGGED_ROMS AS
SELECT ROMS.*, group_concat( group_name, x'0A' ) AS groups
FROM ROMS LEFT JOIN ROM_GROUPS USING( sha1 )
GROUP BY sha1
)#")
      .execNonQuery();

  transaction.commit();
}

// Migration 09: Added remove_borders column to ROMS
inline static void migration_09() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('ROMS')
WHERE name = 'remove_borders'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlTransaction transaction;

  SqlBatch(R"#(
DROP VIEW TAGGED_ROMS;
DROP INDEX ix_rom_by_crc;

CREATE TABLE ROMS_v2(
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
)#")
      .execNonQuery();

  SqlCommand(R"#(
INSERT INTO ROMS_v2(
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
	overclock_cpu,
	overclock_vi,
	widescreen,
	big_eeprom,
	use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	crc32
)
SELECT
	sha1,
	name,
	internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	upscale_texrects,
	0 AS remove_borders,
	emulate_framebuffer,
	n64_depth_compare,
	overclock_cpu,
	overclock_vi,
	widescreen,
	big_eeprom,
	use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	crc32
FROM ROMS
)#")
      .execNonQuery();

  SqlCommand("DROP TABLE ROMS").execNonQuery();

  SqlCommand("ALTER TABLE ROMS_v2 RENAME TO ROMS").execNonQuery();
  SqlCommand("CREATE INDEX ix_rom_by_crc ON ROMS( crc32 )").execNonQuery();

  SqlCommand(R"#(
CREATE VIEW TAGGED_ROMS AS
SELECT ROMS.*, group_concat( group_name, x'0A' ) AS groups
FROM ROMS LEFT JOIN ROM_GROUPS USING( sha1 )
GROUP BY sha1
)#")
      .execNonQuery();

  transaction.commit();
}

// Migration 10: Added input_types column to ROMS
inline static void migration_10() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('ROMS')
WHERE name = 'input_types'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlTransaction transaction;

  SqlBatch(R"#(
DROP VIEW TAGGED_ROMS;
DROP INDEX ix_rom_by_crc;

CREATE TABLE ROMS_v2(
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
	overclock_cpu INTEGER NOT NULL,
	overclock_vi INTEGER NOT NULL,
	widescreen INTEGER NOT NULL,
	big_eeprom INTEGER NOT NULL,
	use_interpreter INTEGER NOT NULL,
	input_mode_id BLOB NOT NULL,
	last_played INTEGER NOT NULL,
	play_time INTEGER NOT NULL,
	crc32 INTEGER NOT NULL,
	input_types BLOB NOT NULL
);
)#")
      .execNonQuery();

  SqlCommand(R"#(
INSERT INTO ROMS_v2(
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
	overclock_cpu,
	overclock_vi,
	widescreen,
	big_eeprom,
	use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	crc32,
	input_types
)
SELECT
	sha1,
	name,
	internal_name,
	emulator,
	parallel_plugin,
	mupen_plugin,
	upscale_texrects,
	0 AS remove_borders,
	emulate_framebuffer,
	n64_depth_compare,
	overclock_cpu,
	overclock_vi,
	widescreen,
	big_eeprom,
	use_interpreter,
	input_mode_id,
	last_played,
	play_time,
	crc32,
	x'00000000' AS input_types
FROM ROMS
)#")
      .execNonQuery();

  SqlCommand("DROP TABLE ROMS").execNonQuery();

  SqlCommand("ALTER TABLE ROMS_v2 RENAME TO ROMS").execNonQuery();
  SqlCommand("CREATE INDEX ix_rom_by_crc ON ROMS( crc32 )").execNonQuery();

  SqlCommand(R"#(
CREATE VIEW TAGGED_ROMS AS
SELECT ROMS.*, group_concat( group_name, x'0A' ) AS groups
FROM ROMS LEFT JOIN ROM_GROUPS USING( sha1 )
GROUP BY sha1
)#")
      .execNonQuery();

  transaction.commit();
}

// Migration 11: Added archived column to RHDC_HACK_VERSIONS
inline static void migration_11() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('RHDC_HACK_VERSIONS')
WHERE name = 'archived'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlTransaction transaction;

  SqlCommand(R"#(
CREATE TABLE RHDC_HACK_VERSIONS_v2(
	sha1 TEXT NOT NULL PRIMARY KEY,
	id TEXT NOT NULL,
	version_name TEXT NOT NULL,
	sort_order INTEGER NOT NULL,
	recommended_plugin INTEGER NOT NULL,
	hack_flags INTEGER NOT NULL,
	archived INTEGER NOT NULL
)
)#")
      .execNonQuery();

  SqlCommand("DROP INDEX ix_hack_versions_by_id").execNonQuery();

  SqlCommand(R"#(
INSERT INTO RHDC_HACK_VERSIONS_v2(
	sha1,
	id,
	version_name,
	sort_order,
	recommended_plugin,
	hack_flags,
	archived
)
SELECT
	sha1,
	id,
	version_name,
	sort_order,
	recommended_plugin,
	hack_flags,
	0 as archived
FROM RHDC_HACK_VERSIONS
)#")
      .execNonQuery();

  SqlCommand("DROP TABLE RHDC_HACK_VERSIONS").execNonQuery();
  SqlCommand("ALTER TABLE RHDC_HACK_VERSIONS_v2 RENAME TO RHDC_HACK_VERSIONS")
      .execNonQuery();
  SqlCommand("CREATE INDEX ix_hack_versions_by_id ON RHDC_HACK_VERSIONS( id )")
      .execNonQuery();

  transaction.commit();
}

// Migration 12: Added sd_card column to ROMS
inline static void migration_12() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('ROMS')
WHERE name = 'sd_card'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand("ALTER TABLE ROMS ADD COLUMN sd_card TEXT NOT NULL DEFAULT( '' )")
      .execNonQuery();
}

// Migration 13: Add gliden_lle_rsp column to ROMS
inline static void migration_13() {
  SqlCommand checkCmd(R"#(
SELECT 1
FROM pragma_table_info('ROMS')
WHERE name = 'gliden_lle_rsp'
)#");

  if (checkCmd.execQuery().moveNext()) {
    return;
  }

  SqlCommand("ALTER TABLE ROMS ADD COLUMN gliden_lle_rsp INTEGER NOT NULL "
             "DEFAULT( 0 )")
      .execNonQuery();
}

void Migration::updateDatabaseSchema() {
  migration_01();
  migration_02();
  migration_03();
  migration_04();
  migration_05();
  migration_06();
  migration_07();
  migration_08();
  migration_09();
  migration_10();
  migration_11();
  migration_12();
  migration_13();
}

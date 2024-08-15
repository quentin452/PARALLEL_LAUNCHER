#ifndef SRC_DB_DATA_PROVIDER_HPP_
#define SRC_DB_DATA_PROVIDER_HPP_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include <set>
#include "src/rhdc/core/hack.hpp"
#include "src/core/logging.hpp"
#include "src/core/version.hpp"
#include "src/core/rom.hpp"
#include "src/types.hpp"

namespace DataProvider {

	extern void addRomInfo( const RomInfo &romInfo ) noexcept;
	extern void addRomFile( const RomFile &romFile );

	extern void addGroup( const string &groupName ) noexcept;
	extern void deleteGroup( const string &groupName ) noexcept;
	extern void renameGroup( const string &oldName, const string &newName ) noexcept;

	extern void updateRomHeaderInfo( const string &sha1, const string &name, uint crc32, const N64InputTypes &inputTypes ) noexcept;
	extern void updateRomEmulator( const string &sha1, EmulatorCore core ) noexcept;
	extern void updateRomParallelPlugin( const string &sha1, GfxPlugin plugin ) noexcept;
	extern void updateRomMupenPlugin( const string &sha1, GfxPlugin plugin ) noexcept;
	extern void updateRomCpuOverclocking( const string &sha1, bool overclock ) noexcept;
	extern void updateRomViOverclocking( const string &sha1, bool overclock ) noexcept;
	extern void updateRomWidescreen( const string &sha1, WidescreenMode widescreen ) noexcept;
	extern void updateRomBigEeprom( const string &sha1, bool bigEeprom ) noexcept;
	extern void updateRomUseInterpreter( const string &sha1, bool useInterpreter ) noexcept;
	extern void updateRomInputMode( const string &sha1, const Uuid &inputModeId ) noexcept;
	extern void updatePlayTime( const string &sha1, int64 lastPlayed, int64 playTime ) noexcept;
	extern void updateSdCard( const string &sha1, const string &card ) noexcept;
	extern void renameRom( const string &sha1, const string &name ) noexcept;
	extern void updatePluginSettings(
		const string &sha1,
		EmulatorCore core,
		GfxPlugin parallelPlugin,
		GfxPlugin mupenPlugin,
		bool upscaleTexrects,
		bool removeBorders,
		bool emulateFramebuffer,
		bool accurateDepthCompare,
		bool glidenParallelRsp
	);

	extern bool tryFetchRomByHash( const string &sha1, bool includeGroups, RomInfo *romInfo ) noexcept;
	extern bool tryFetchRomByPath( const fs::path &filePath, bool includeGroups, RomInfo *romInfo, RomFile *romFile = nullptr );
	extern bool tryFetchRomFile( const fs::path &filePath, RomFile *romFile );

	extern bool tryFetchRomPathWithCrc32( uint crc32, fs::path &filePath );
	extern bool tryFetchRomPathWithSha1( const string &sha1, fs::path &filePath );
	extern bool tryFetchRomFileWithSha1( const string &sha1, const string &playlistHint, RomFile *romFile );

	extern int countRomFilesWithSha1( const string &sha1 ) noexcept;
	extern int countRomsInGroup( const string &groupName ) noexcept;
	extern int countRomFiles() noexcept;

	extern bool romWithSha1Exists( const string &sha1 ) noexcept;
	extern bool romWithPathExists( const fs::path &filePath );

	extern std::set<string> fetchPersistentGroups();
	extern std::vector<string> fetchAllGroups();
	extern std::vector<RomFile> fetchLocalRomFiles();
	extern std::vector<RomFile> fetchAllRomFiles();
	extern HashMap<string,RomInfo> fetchAllRomInfo();
	extern HashSet<string> fetchAllKnownChecksums();

	extern std::vector<fs::path> fetchManualRomPaths();
	extern void addManualRomPath( const fs::path &path );
	extern void deleteManualRomPath( const fs::path &path );
	extern void deleteAllManualRomPaths() noexcept;

	extern void updateRomFile( const RomFile &romFile ) noexcept;

	extern void cloneRomInfo(
		const string &oldSha1,
		const string &newSha1,
		const string &internalName,
		uint crc32,
		const N64InputTypes &inputTypes
	) noexcept;

	extern void deleteRomPath( const fs::path &filePath );
	extern void deleteRomInfo( const string &sha1 ) noexcept;

	extern void addToGroup( const string &sha1, const string &groupName ) noexcept;
	extern void removeFromGroup( const string &sha1, const string &groupName ) noexcept;

	extern void setRomParallelTexRectUpscaling( const string &sha1, bool enabled ) noexcept;
	extern void setRomParallelRemoveBorders( const string &sha1, bool enabled ) noexcept;
	extern void setRomGlidenFrameBufferEmulation( const string &sha1, bool enabled ) noexcept;
	extern void setRomGlidenAccurateDepthCompare( const string &sha1, bool enabled ) noexcept;
	extern void setRomGlidenRspEmulation( const string &sha1, bool enabled ) noexcept;
	extern void setDefaultParallelTexRectUpscaling( bool enabled ) noexcept;
	extern void setDefaultParallelRemoveBorders( bool enabled ) noexcept;
	extern void setDefaultGlidenFrameBufferEmulation( bool enabled ) noexcept;
	extern void setDefaultGlidenAccurateDepthCompare( bool enabled ) noexcept;
	extern void setAllParallelTexRectUpscaling( bool enabled ) noexcept;
	extern void setAllGlidenFrameBufferEmulation( bool enabled ) noexcept;
	extern void setAllGlidenAccurateDepthCompare( bool enabled ) noexcept;

	extern string getLibplSdCard( const string &uid ) noexcept;
	extern void registerLibplSdCard( const string &uid, const string &name ) noexcept;
	extern void removeLibplSdCard( const string &uid ) noexcept;

	/* rhdc */

	extern std::vector<string> getPendingPluginChecks() noexcept;
	extern void addPendingPluginCheck( const string &sha1, bool force = false ) noexcept;
	extern bool hasPendingPluginCheck( const string &sha1 ) noexcept;
	extern void removePendingPluginCheck( const string &sha1 ) noexcept;
	extern void removePendingPluginChecks() noexcept;

	extern void addOrUpdateRhdcHack( const RhdcHackInfo &hack ) noexcept;
	extern void addRhdcHackAuthor( const string &hackId, const string &author ) noexcept;
	extern void clearRhdcHackVersions( const string &hackId ) noexcept;
	extern void addRhdcHackVersion(
		const string &hackId,
		const string &sha1,
		const string &name,
		int sortOrder,
		GfxPlugin recommendedPlugin,
		RhdcHackFlag hackFlags,
		bool archived
	) noexcept;

	extern void clearRhdcHacks() noexcept;
	extern void removeOnlineRom( const string &hackId ) noexcept;

	extern void updateRhdcProgress( const string &hackId, const RhdcHackProgress &progress ) noexcept;
	extern void updateRhdcHackRating( const string &hackId, RhdcRating quality, ubyte difficulty ) noexcept;
	extern void updateRhdcStarProgress( const string &hackId, ushort stars, bool completed ) noexcept;
	extern void suggestRhdcHackPlayTime( const string &hackId, int64 playTime ) noexcept;
	extern int64 getPlayTimeByHackId( const string &hackId ) noexcept;

	extern HashMap<string,BasicRhdcHack> fetchFollowedRhdcHacks();
	extern HashMap<string,string> fetchAllRhdcHackVersions();
	extern HashMap<string,StarpowerExt> fetchAllStarpower();

	extern bool tryFetchRhdcHackByChecksum( const string &sha1, RhdcHack *hack ) noexcept;
	extern bool tryFetchHackIdByChecksum( const string &sha1, string &hackId ) noexcept;

	extern std::set<string> fetchGroupsForRhdcHack( const string &hackId );
	extern void addRhdcHackToGroup( const string &hackId, const string &groupName ) noexcept;
	extern void removeRhdcHackFromGroup( const string &hackId, const string &groupName ) noexcept;

	extern bool hasRhdcData() noexcept;
	extern void clearAllRhdcData() noexcept;
	extern void forgetHack( const string &hackId ) noexcept;

	extern std::vector<RhdcHackExt> fetchExtRhdcHacks( const string &searchText );
	extern std::vector<RhdcHackExt> fetchExtRhdcHacksInList( const string &playlist, const string &searchText );
	extern std::vector<RhdcHackVersion> fetchRhdcHackVersions( const string &hackId );
	extern std::vector<fs::path> fetchRomPathsForRhdcHack( const string &hackId );
	extern std::vector<string> fetchRhdcPlaylists();

	extern string getLastPlayedVersion( const string &hackId ) noexcept;
	extern void setLastPlayedVersion( const string &hackId, const string &sha1 ) noexcept;

	extern RhdcSettingHints getRhdcHints( const string &sha1 ) noexcept;
	extern string getVersionNameFromRomPath( const fs::path &romPath ) noexcept;

	extern bool groupSaveEnabled( const string &hackId ) noexcept;
	extern void enableGroupSave( const string &hackId ) noexcept;
	extern void disableGroupSave( const string &hackId ) noexcept;

	extern bool tryGetSaveGroup( const string &sha1, string &hackId ) noexcept;
	extern bool tryGetSaveGroupForRomPath( const fs::path &romPath, string &hackId ) noexcept;
	extern bool isSaveGroupEmpty( const string &hackId ) noexcept;
	extern void addAllVersionsToSaveGroup( const string &hackId ) noexcept;
	extern void removeFromSaveGroup( const string &hackId, const string &sha1 ) noexcept;

	/* logging */

	extern void addLogEntry( const LogEntry &logEntry ) noexcept;
	extern std::vector<LogEntry> fetchLogs();

	extern void addCrashLog( int64 timestamp, const string &report ) noexcept;
	extern void setCrashLogSentDate( int64 crashedAt, int64 sentAt ) noexcept;
	extern std::map<int64,string> getUnreportedCrashLogs();
	extern std::map<int64,string> getCrashLogsForVersion( const Version &appVersion );
	extern std::map<int64,string> getAllCrashLogs();

}



#endif /* SRC_DB_DATA_PROVIDER_HPP_ */

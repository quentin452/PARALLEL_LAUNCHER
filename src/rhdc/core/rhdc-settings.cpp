#include "src/rhdc/core/rhdc-settings.hpp"

#include "src/polyfill/base-directory.hpp"
#include "src/core/steam-deck.hpp"

const RhdcSettings &RhdcSettings::Default() {
	static const RhdcSettings s_default = {
		/* downloadDirectory */ BaseDir::home() / _NFS("Downloads") / _NFS("rhdc-hacks"),
		/* enableStarDisplay */ true,
		/* checkAllSaveSlots */ false,
		/* preferHle */ false,
		/* enableDefaultLayouts */ true,
		/* ignoreWidescreenHint */ false
	};
	return s_default;
}

static constexpr char P_DOWNLOAD_DIR[] = "download_directory";
static constexpr char P_ENABLE_STAR_DISPLAY[] = "enable_star_display";
static constexpr char P_ALL_SAVE_SLOTS[] = "check_all_save_slots";
static constexpr char P_PREFER_HLE[] = "perfer_hle";
static constexpr char P_ENABLE_DEFAULT_LAYOUTS[] = "enable_default_layouts";
static constexpr char P_IGNORE_WIDESCREEN[] = "ignore_widescreen_hint";

template<> void JsonSerializer::serialize<RhdcSettings>( JsonWriter &jw, const RhdcSettings &obj ) {
	jw.writeObjectStart();
	jw.writeProperty( P_DOWNLOAD_DIR, obj.downloadDirectory.u8string() );
	jw.writeProperty( P_ENABLE_STAR_DISPLAY, obj.enableStarDisplay );
	jw.writeProperty( P_ALL_SAVE_SLOTS, obj.checkAllSaveSlots );
	jw.writeProperty( P_PREFER_HLE, obj.preferHle );
	jw.writeProperty( P_ENABLE_DEFAULT_LAYOUTS, obj.enableDefaultLayouts );
	jw.writeProperty( P_IGNORE_WIDESCREEN, obj.ignoreWidescreenHint );
	jw.writeObjectEnd();
}

template<> RhdcSettings JsonSerializer::parse<RhdcSettings>( const Json &json ) {
	const string downloadDir = json[P_DOWNLOAD_DIR].getOrDefault<string>( "" );
	return RhdcSettings {
		downloadDir.empty() ? RhdcSettings::Default().downloadDirectory : fs::to_path( downloadDir ),
		json[P_ENABLE_STAR_DISPLAY].getOrDefault<bool>( RhdcSettings::Default().enableStarDisplay ),
		json[P_ALL_SAVE_SLOTS].getOrDefault<bool>( RhdcSettings::Default().checkAllSaveSlots ),
		SteamDeck::isSteamDeck() ? false : json[P_PREFER_HLE].getOrDefault<bool>( RhdcSettings::Default().preferHle ),
		json[P_ENABLE_DEFAULT_LAYOUTS].getOrDefault<bool>( RhdcSettings::Default().enableDefaultLayouts ),
		json[P_IGNORE_WIDESCREEN].getOrDefault<bool>( RhdcSettings::Default().ignoreWidescreenHint )
	};
}

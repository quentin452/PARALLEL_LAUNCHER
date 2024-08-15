#include "src/core/file-controller.hpp"

#include "src/polyfill/crash.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/hotkeys.hpp"
#include "src/core/logging.hpp"
#include "src/ui/error-notifier.hpp"
#include "src/rhdc/core/rhdc-settings.hpp"

#ifdef _WIN32
#include "src/polyfill/windows/unicode.hpp"
#endif

template<class T> class FileCache {

	private:
	T m_value;
	const fs::path m_filePath;
	void (*const m_setter)(JsonWriter&, const T &);

	void commit() const {
		try {
			OutputFile file( m_filePath, false );
			file.exceptions( std::ios_base::badbit );
			JsonWriter writer( &file, true );
			m_setter( writer, m_value );
			file.flush();
		} catch( const std::exception &exception ) {
			string msg = "Failed to save data to '"s + m_filePath.u8string() + "'.\n";
			if( dynamic_cast<const JsonWriterException*>( &exception ) ) {
				msg += "Failed to serialize data to JSON. Please report this bug!\nDetails: ";
			} else if( dynamic_cast<const fs::filesystem_error*>( &exception ) ) {
				msg += "Filesystem Error: ";
			} else if( dynamic_cast<const std::ios_base::failure*>( &exception ) ) {
				msg += "I/O Error: ";
			}
			msg += exception.what();
			logError( msg );
			ErrorNotifier::alert( msg );
		}
	}

	public:
	FileCache(
		const fs::path &filePath,
		const T &defaultValue,
		T (*getter)(const Json&),
		void (*setter)(JsonWriter&, const T &)
	) : m_filePath( filePath ), m_setter( setter ) {
		if( !fs::isRegularFileSafe( filePath ) ) {
			m_value = defaultValue;
		} else try {
			InputFile file( filePath, false );
			m_value = getter( Json::parse( file ) );
		} catch( const std::exception &exception ) {
			string msg = "Failed to load data from '"s + m_filePath.u8string() + "'.\n";
			if( dynamic_cast<const JsonReaderException*>( &exception ) ) {
				msg += "Failed to parse JSON data. Please report this bug!\nDetails: ";
			} else if( dynamic_cast<const fs::filesystem_error*>( &exception ) ) {
				msg += "Filesystem Error: ";
			} else if( dynamic_cast<const std::ios_base::failure*>( &exception ) ) {
				msg += "I/O Error: ";
			}
			msg += exception.what();
			logError( msg );
			ErrorNotifier::alert( msg );
			m_value = defaultValue;
		}
	}

	~FileCache() {}

	inline const T &get() const {
		return m_value;
	}

	inline void set( const T &value ) {
		m_value = value;
		commit();
	}

	inline void set( T &&value ) {
		m_value = std::move( value );
		commit();
	}

};

template<class T> static std::vector<T> parseJsonArray( const Json &json ) {
	const JArray &jsonArray = json.array();

	std::vector<T> parsedArray;
	parsedArray.reserve( jsonArray.size() );
	for( const Json &item : jsonArray ) {
		parsedArray.push_back( JsonSerializer::parse<T>( item ) );
	}
	return parsedArray;
}

template<class T> static void serializeToJsonArray( JsonWriter &writer, const std::vector<T> &array ) {
	writer.writeArrayStart();
	for( const T &item : array ) {
		JsonSerializer::serialize<T>( writer, item );
	}
	writer.writeArrayEnd();
}

static std::map<string, ControllerProfile> parseProfiles( InputDriver driver, const Json &json ) {
	const JArray &jsonArray = json.array();

	std::map<string, ControllerProfile> profiles;
	for( const Json &profileJson : jsonArray ) {
		ControllerProfile profile = JsonSerializer::parse<ControllerProfile>( profileJson );
		profiles[profile.name] = profile;
	}

	for( const ControllerProfile &defaultProfile : DefaultProfile::getAll( driver ) ) {
		profiles[defaultProfile.name] = defaultProfile;
	}

	return profiles;
}

static void serializeProfiles( InputDriver driver, JsonWriter &writer, const std::map<string, ControllerProfile> &profiles ) {
	writer.writeArrayStart();
	for( const auto &i : profiles ) {
		if( !DefaultProfile::exists( driver, i.first ) ) {
			JsonSerializer::serialize<ControllerProfile>( writer, i.second );
		}
	}
	writer.writeArrayEnd();
}

static std::map<string, ControllerProfile> parseHidProfiles( const Json &json ) {
	return parseProfiles( InputDriver::HID, json );
}

static std::map<string, ControllerProfile> parseSdlProfiles( const Json &json ) {
	return parseProfiles( InputDriver::SDL, json );
}

static std::map<string, ControllerProfile> parseDirectInputProfiles( const Json &json ) {
	return parseProfiles( InputDriver::DirectInput, json );
}

static std::map<string, ControllerProfile> parseJoyDevProfiles( const Json &json ) {
	return parseProfiles( InputDriver::JoyDev, json );
}

static void serializeHidProfiles( JsonWriter &writer, const std::map<string, ControllerProfile> &profiles ) {
	serializeProfiles( InputDriver::HID, writer, profiles );
}

static void serializeSdlProfiles( JsonWriter &writer, const std::map<string, ControllerProfile> &profiles ) {
	serializeProfiles( InputDriver::SDL, writer, profiles );
}

static void serializeDirectInputProfiles( JsonWriter &writer, const std::map<string, ControllerProfile> &profiles ) {
	serializeProfiles( InputDriver::DirectInput, writer, profiles );
}

static void serializeJoyDevProfiles( JsonWriter &writer, const std::map<string, ControllerProfile> &profiles ) {
	serializeProfiles( InputDriver::JoyDev, writer, profiles );
}

static std::map<Uuid, InputMode> parseInputModes( const Json &json ) {
	const JArray &jsonArray = json.array();

	std::map<Uuid, InputMode> inputModes;
	for( const Json &profileJson : jsonArray ) {
		InputMode inputMode = JsonSerializer::parse<InputMode>( profileJson );
		inputModes[inputMode.id] = inputMode;
	}
	inputModes[DefaultInputModes::Normal.id] = DefaultInputModes::Normal;
	inputModes[DefaultInputModes::DualAnalog.id] = DefaultInputModes::DualAnalog;
	inputModes[DefaultInputModes::GoldenEye.id] = DefaultInputModes::GoldenEye;
	inputModes[DefaultInputModes::Clone.id] = DefaultInputModes::Clone;
	return inputModes;
}

static void serializeInputModes( JsonWriter &writer, const std::map<Uuid, InputMode> &inputModes ) {
	writer.writeArrayStart();
	for( const auto &i : inputModes ) {
		JsonSerializer::serialize<InputMode>( writer, i.second );
	}
	writer.writeArrayEnd();
}

static HashMap<Uuid,string> parseMappings( const Json &json ) {
	const JArray &jsonArray = json.array();

	HashMap<Uuid,string> mappings;
	for( const Json &mappingJson : jsonArray ) {
		const Uuid deviceId = Uuid::parse( mappingJson["device"].get<string>() );
		mappings[deviceId] = mappingJson["profile"].get<string>();
	}
	return mappings;
}

static void serializeMappings( JsonWriter &writer, const HashMap<Uuid,string> &mappings ) {
	writer.writeArrayStart();
	for( const auto &i : mappings ) {
		writer.writeObjectStart();
		writer.writeProperty( "device", i.first.toString() );
		writer.writeProperty( "profile", i.second );
		writer.writeObjectEnd();
	}
	writer.writeArrayEnd();
}

static inline void insertXYButtonBindingsIfMissing( JArray& bindingsJson ) {
	// Check if this json is for an old version before the X and Y buttons were added
	bool migratingFromOldJson = bindingsJson.size() == 43u;
	// If it is, insert two dummy bindings where the X and Y buttons were added
	if( migratingFromOldJson ) {
		// X and Y were inserted at position 18, so insert two null json values at that index
		bindingsJson.emplace( bindingsJson.begin() + 18u, Json((int64)KeyId::INVALID) );
		bindingsJson.emplace( bindingsJson.begin() + 18u, Json((int64)KeyId::INVALID) );
	}
}

static std::vector<KeyId> parseHotkeys( const Json &json ) {
	JArray bindingsJson = json.array();

	if( bindingsJson.size() == 43 ) {
		// Migrate from old format
		bindingsJson.emplace( bindingsJson.begin() + 18u, (int64)Hotkeys::Default[(ubyte)ControllerAction::ButtonX] );
		bindingsJson.emplace( bindingsJson.begin() + 19u, (int64)Hotkeys::Default[(ubyte)ControllerAction::ButtonY] );
	}

	if( bindingsJson.size() == 45 ) {
		bindingsJson.push_back( Json((int64)Hotkeys::Default[(ubyte)Hotkey::GrabMouse]) );
	}

	if( bindingsJson.size() != (size_t)Hotkey::NUM_HOTKEYS ) {
		throw std::range_error( "Array of hotkeys is the wrong size" );
	}

	std::vector<KeyId> hotkeys;
	hotkeys.reserve( (size_t)Hotkey::NUM_HOTKEYS );
	for( const Json &hotkey : bindingsJson ) {
		hotkeys.push_back( hotkey.get<KeyId>() );
	}
	return hotkeys;
}

static void serializeHotkeys( JsonWriter &writer, const std::vector<KeyId> &hotkeys ) {
	writer.writeArrayStart();
	for( KeyId hotkey : hotkeys ) {
		writer.writeNumber( (int64)hotkey );
	}
	writer.writeArrayEnd();
}

static string parseString( const Json &json ) {
	return json.getOrDefault<string>( "" );
}

static void serializeString( JsonWriter &writer, const string &str ) {
	writer.writeString( str );
}

static FileCache<AppSettings> &appSettingsCache() {
	static FileCache<AppSettings> s_cache = FileCache<AppSettings>(
		BaseDir::config() / _NFS("settings.cfg"),
		AppSettings::Default,
		JsonSerializer::parse<AppSettings>,
		JsonSerializer::serialize<AppSettings>
	);
	return s_cache;
}

static FileCache<std::vector<RomSource>> &romSourcesCache() {
	static FileCache<std::vector<RomSource>> s_cache = FileCache<std::vector<RomSource>>(
		BaseDir::data() / _NFS("sources.json"),
		std::vector<RomSource>(),
		parseJsonArray<RomSource>,
		serializeToJsonArray<RomSource>
	);
	return s_cache;
}

static inline std::pair<string, ControllerProfile> makeProfileEntry( InputDriver driver, ControllerType controller ) {
	const ControllerProfile &profile = DefaultProfile::get( driver, controller );
	return { profile.name, profile };
}

static FileCache<std::map<string, ControllerProfile>> &profilesCache( InputDriver driver ) {
	switch( driver ) {
		case InputDriver::HID: {
			static FileCache<std::map<string, ControllerProfile>> s_cache = FileCache<std::map<string, ControllerProfile>>(
				BaseDir::data() / _NFS("profiles-hid.json"),
				{
					makeProfileEntry( InputDriver::HID, ControllerType::XBox360 )
				},
				parseHidProfiles,
				serializeHidProfiles
			);
			return s_cache;
		}
		case InputDriver::SDL: {
			static FileCache<std::map<string, ControllerProfile>> s_cache = FileCache<std::map<string, ControllerProfile>>(
				BaseDir::data() / _NFS("profiles-sdl.json"),
				{
					makeProfileEntry( InputDriver::SDL, ControllerType::XBox360 ),
					makeProfileEntry( InputDriver::SDL, ControllerType::Gamecube ),
					makeProfileEntry( InputDriver::SDL, ControllerType::Nintendo64 )
					//makeProfileEntry( InputDriver::SDL, ControllerType::SwitchPro )
				},
				parseSdlProfiles,
				serializeSdlProfiles
			);
			return s_cache;
		}
		case InputDriver::DirectInput: {
			static FileCache<std::map<string, ControllerProfile>> s_cache = FileCache<std::map<string, ControllerProfile>>(
				BaseDir::data() / _NFS("profiles-dinput.json"),
				{
					makeProfileEntry( InputDriver::DirectInput, ControllerType::XBox360 ),
					makeProfileEntry( InputDriver::DirectInput, ControllerType::Gamecube )
				},
				parseDirectInputProfiles,
				serializeDirectInputProfiles
			);
			return s_cache;
		}
		case InputDriver::JoyDev: {
			static FileCache<std::map<string, ControllerProfile>> s_cache = FileCache<std::map<string, ControllerProfile>>(
				BaseDir::data() / _NFS("profiles-joydev.json"),
				{
					makeProfileEntry( InputDriver::JoyDev, ControllerType::XBox360 ),
					makeProfileEntry( InputDriver::JoyDev, ControllerType::Gamecube ),
					makeProfileEntry( InputDriver::JoyDev, ControllerType::SwitchPro )
				},
				parseJoyDevProfiles,
				serializeJoyDevProfiles
			);
			return s_cache;
		}
		default: {
			pl_assert( false );
			return profilesCache( InputDriver::SDL );
		}
	}


}

static FileCache<std::map<Uuid, InputMode>> &inputModesCache() {
	static FileCache<std::map<Uuid, InputMode>> s_cache = FileCache<std::map<Uuid, InputMode>>(
		BaseDir::data() / _NFS("input_modes.json"),
		{
			{ DefaultInputModes::Normal.id, DefaultInputModes::Normal },
			{ DefaultInputModes::DualAnalog.id, DefaultInputModes::DualAnalog },
			{ DefaultInputModes::GoldenEye.id, DefaultInputModes::GoldenEye },
			{ DefaultInputModes::Clone.id, DefaultInputModes::Clone }
		},
		parseInputModes,
		serializeInputModes
	);
	return s_cache;
}

static FileCache<HashMap<Uuid,string>> &mappingsCache( InputDriver driver ) {
	switch( driver ) {
		case InputDriver::HID: {
			static FileCache<HashMap<Uuid,string>> s_cache = FileCache<HashMap<Uuid,string>>(
				BaseDir::data() / _NFS("devices-hid.json"),
				HashMap<Uuid,string>(),
				parseMappings,
				serializeMappings
			);
			return s_cache;
		}
		case InputDriver::SDL: {
			static FileCache<HashMap<Uuid,string>> s_cache = FileCache<HashMap<Uuid,string>>(
				BaseDir::data() / _NFS("devices-sdl.json"),
				HashMap<Uuid,string>(),
				parseMappings,
				serializeMappings
			);
			return s_cache;
		}
		case InputDriver::DirectInput: {
			static FileCache<HashMap<Uuid,string>> s_cache = FileCache<HashMap<Uuid,string>>(
				BaseDir::data() / _NFS("devices-dinput.json"),
				HashMap<Uuid,string>(),
				parseMappings,
				serializeMappings
			);
			return s_cache;
		}
		case InputDriver::JoyDev: {
			static FileCache<HashMap<Uuid,string>> s_cache = FileCache<HashMap<Uuid,string>>(
				BaseDir::data() / _NFS("devices-joydev.json"),
				HashMap<Uuid,string>(),
				parseMappings,
				serializeMappings
			);
			return s_cache;
		}
		default: {
			pl_assert( false );
			return mappingsCache( InputDriver::SDL );
		}
	}
}


static FileCache<UiState> &uiStateCache() {
	static FileCache<UiState> s_cache = FileCache<UiState>(
		BaseDir::cache() / _NFS("ui-state.json"),
		UiState::Default,
		JsonSerializer::parse<UiState>,
		JsonSerializer::serialize<UiState>
	);
	return s_cache;
}

static FileCache<std::vector<KeyId>> &hotkeysCache() {
	static FileCache<std::vector<KeyId>> s_cache = FileCache<std::vector<KeyId>>(
		BaseDir::config() / _NFS("hotkeys.json"),
		std::vector<KeyId>( Hotkeys::Default, Hotkeys::Default + (int)Hotkey::NUM_HOTKEYS ),
		parseHotkeys,
		serializeHotkeys
	);
	return s_cache;
}

static FileCache<InstalledVersionsInfo> &installedVersionsCache() {
	static FileCache<InstalledVersionsInfo> s_cache = FileCache<InstalledVersionsInfo>(
		BaseDir::data() / _NFS("cores.json"),
		InstalledVersionsInfo::Default,
		JsonSerializer::parse<InstalledVersionsInfo>,
		JsonSerializer::serialize<InstalledVersionsInfo>
	);
	return s_cache;
}

static FileCache<RhdcSettings> &rhdcSettingsCache() {
	static FileCache<RhdcSettings> s_cache = FileCache<RhdcSettings>(
		BaseDir::config() / _NFS("rhdc.cfg"),
		RhdcSettings::Default(),
		JsonSerializer::parse<RhdcSettings>,
		JsonSerializer::serialize<RhdcSettings>
	);
	return s_cache;
}

static FileCache<string> &lastControllerProfileNameCache( InputDriver driver ) {
	switch( driver ) {
		case InputDriver::HID: {
			static FileCache<string> s_cache = FileCache<string>(
				BaseDir::cache() / _NFS("last_profile-hid.json"),
				"",
				parseString,
				serializeString
			);
			return s_cache;
		}
		case InputDriver::SDL: {
			static FileCache<string> s_cache = FileCache<string>(
				BaseDir::cache() / _NFS("last_profile-sdl.json"),
				"",
				parseString,
				serializeString
			);
			return s_cache;
		}
		case InputDriver::DirectInput: {
			static FileCache<string> s_cache = FileCache<string>(
				BaseDir::cache() / _NFS("last_profile-dinput.json"),
				"",
				parseString,
				serializeString
			);
			return s_cache;
		}
		case InputDriver::JoyDev: {
			static FileCache<string> s_cache = FileCache<string>(
				BaseDir::cache() / _NFS("last_profile-joydev.json"),
				"",
				parseString,
				serializeString
			);
			return s_cache;
		}
		default: {
			pl_assert( false );
			return lastControllerProfileNameCache( InputDriver::SDL );
		}
	}
}

const AppSettings &FileController::loadAppSettings() {
	return appSettingsCache().get();
}

void FileController::saveAppSettings( const AppSettings &settings ) {
	appSettingsCache().set( settings );
}

const std::vector<RomSource> &FileController::loadRomSources() {
	return romSourcesCache().get();
}

void FileController::saveRomSources( const std::vector<RomSource> &sources ) {
	romSourcesCache().set( sources );
}

const std::map<string, ControllerProfile> &FileController::loadControllerProfiles( InputDriver driver ) {
	return profilesCache( driver ).get();
}

void FileController::saveControllerProfiles( InputDriver driver, const std::map<string, ControllerProfile> &profiles ) {
	profilesCache( driver ).set( profiles );
}

const HashMap<Uuid,string> &FileController::loadControllerMappings( InputDriver driver ) {
	return mappingsCache( driver ).get();
}

void FileController::saveControllerMappings( InputDriver driver, const HashMap<Uuid,string> &mappings ) {
	mappingsCache( driver ).set( mappings );
}

const UiState &FileController::loadUiState() {
	return uiStateCache().get();
}

void FileController::saveUiState( const UiState &state ) {
	uiStateCache().set( state );
}

const std::vector<KeyId> &FileController::loadHotkeys() {
	return hotkeysCache().get();
}

void FileController::saveHotkeys( const std::vector<KeyId> &hotkeys ) {
	hotkeysCache().set( hotkeys );
}

const std::map<Uuid,InputMode> &FileController::loadInputModes() {
	return inputModesCache().get();
}

void FileController::saveInputModes( const std::map<Uuid,InputMode> &modes ) {
	inputModesCache().set( modes );
}

const InstalledVersionsInfo &FileController::loadInstalledVersions() {
	return installedVersionsCache().get();
}

void FileController::saveInstalledVersions( const InstalledVersionsInfo &versions ) {
	installedVersionsCache().set( versions );
}

const RhdcSettings &FileController::loadRhdcSettings() {
	return rhdcSettingsCache().get();
}

void FileController::saveRhdcSettings( const RhdcSettings &settings ) {
	rhdcSettingsCache().set( settings );
}

const ControllerProfile &FileController::loadLastControllerProfile( InputDriver driver ) {
	const string &profileName = lastControllerProfileNameCache( driver ).get();
	const std::map<string, ControllerProfile> &profiles = FileController::loadControllerProfiles( driver );
	const auto profile = profiles.find( profileName );
	if( profile == profiles.end() ) {
		return DefaultProfile::get( driver );
	} else {
		return profile->second;
	}
}

void FileController::saveLastControllerProfile( InputDriver driver, const string &name ) {
	lastControllerProfileNameCache( driver ).set( name );
}

#include "src/core/migration.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/hotkeys.hpp"
#include "src/core/json.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/qthread.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/special-groups.hpp"
#include "src/core/updates.hpp"
#include "src/core/utf8.hpp"
#include "src/db/data-provider.hpp"
#include "src/db/sqlite.hpp"
#include "src/db/transaction.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/ui/core-installer.hpp"
#include <QCloseEvent>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include <memory>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#ifdef _WIN32
#include "src/core/save-converter.hpp"
#include "src/polyfill/windows/unicode.hpp"
#endif

class StubbornProgressDialog : public QProgressDialog {

private:
  bool *m_migrationFinished;

public:
  StubbornProgressDialog(bool *migrationStatus)
      : QProgressDialog(nullptr), m_migrationFinished(migrationStatus) {
    setModal(true);
    setWindowModality(Qt::ApplicationModal);
    setLabelText("Computing ROM Checksums...");
    setCancelButton(nullptr);
    setAutoClose(false);

    QProgressBar *progressBar = new QProgressBar();
    progressBar->setOrientation(Qt::Horizontal);
    progressBar->setFormat("%v/%m");
    progressBar->setTextVisible(true);

    setBar(progressBar);
    setRange(0, 0);
  }

  virtual ~StubbornProgressDialog() {}

protected:
  virtual void closeEvent(QCloseEvent *event) override {
    if (*m_migrationFinished) {
      QProgressDialog::closeEvent(event);
    } else {
      event->ignore();
    }
  }
};

static inline string removeNewlines(const string &str) {
  string safeString;
  safeString.reserve(str.size());
  for (char c : str) {
    if (c != '\n')
      safeString += c;
  }
  return safeString;
}

static inline void processRom(const Json &json,
                              HashMap<string, RomInfo> &romInfoTable,
                              std::vector<RomFile> &romFiles,
                              [[maybe_unused]] bool fixEncoding) {
  string romName = json["name"].getOrDefault<string>("");
  if (romName.empty())
    return;

  string romPathString = json["file_path"].getOrDefault<string>("");
  if (romPathString.empty())
    return;

#ifdef _WIN32
  if (fixEncoding) {
    romPathString = Unicode::latinToUtf8(romPathString);
  }
#endif

  const fs::path romPath = fs::to_path(romPathString);
  if (!fs::existsSafe(romPath))
    return;

  Uuid inputModeId;
  if (!Uuid::tryParse(json["input_mode"].getOrDefault<string>(""),
                      inputModeId)) {
    inputModeId = DefaultInputModes::Normal.id;
  }

  std::set<string> groups;
  if (json["tags"].isArray()) {
    for (const Json &group : json["tags"].array()) {
      string groupName = removeNewlines(group.getOrDefault<string>(""));
      if (groupName.empty())
        continue;
      if (groupName == "Favourites")
        groupName = SpecialGroups::Favourites;
      groups.insert(std::move(groupName));
    }
  }

  const int64 lastPlayed = json["last_played"].getOrDefault<int64>(0);
  const int64 playTime = json["play_time"].getOrDefault<int64>(0);

  string sha1 = Sha1::compute(romPath);
  if (romInfoTable.count(sha1) > 0) {
    RomInfo &romInfo = romInfoTable[sha1];
    if (romInfo.lastPlayed < lastPlayed)
      romInfo.lastPlayed = lastPlayed;
    romInfo.playTime += playTime;
    romInfo.groups.insert(groups.begin(), groups.end());
  } else {
    romInfoTable[sha1] = RomInfo{
        sha1,
        std::move(romName),
        RomUtil::getInternalName(romPath),
        json["emulator_core"].getOrDefault<EmulatorCore>(
            EmulatorCore::ParallelN64),
        json["gfx_plugin"].getOrDefault<GfxPlugin>(GfxPlugin::UseDefault),
        json["gfx_plugin_mupen"].getOrDefault<GfxPlugin>(GfxPlugin::UseDefault),
        false,
        false,
        true,
        true,
        false,
        lastPlayed,
        playTime,
        std::move(groups),
        json["overclock_cpu"].getOrDefault<bool>(true),
        json["overclock_vi"].getOrDefault<bool>(false),
        RomUtil::getCrc32(romPath),
        std::move(inputModeId),
        json["widescreen"].getOrDefault(false) ? WidescreenMode::Stretched
                                               : WidescreenMode::Off,
        false,
        false,
        RomUtil::readControllerTypes(romPath),
        ""};
  }

  romFiles.push_back({romPath, RomUtil::getLastModified(romPath), sha1, true});
}

static inline void migrateRomsToSqlite(bool fixEncoding) {
  const fs::path filePath = BaseDir::data() / _NFS("roms.json");
  if (!fs::isRegularFileSafe(filePath))
    return;

  InputFile romListFile(filePath, false);
  const Json romListJson = Json::parse(romListFile);
  romListFile.close();

  if (!romListJson.isArray()) {
    fs::error_code err;
    fs::remove(filePath, err);
  }

  const JArray &romList = romListJson.array();
  if (romList.empty()) {
    fs::error_code err;
    fs::remove(filePath, err);
  }

  bool migrationFinished = false;
  std::shared_ptr<bool> dialogExists = std::shared_ptr<bool>(new bool(true));
  StubbornProgressDialog dialog(&migrationFinished);
  dialog.setRange(0, (int)romList.size());

  HashMap<string, RomInfo> romInfoTable;
  std::vector<RomFile> romFiles;
  romFiles.reserve(romList.size());

  std::thread migrationWorker([&]() {
    int romsProcessed = 0;
    for (const Json &rom : romList) {
      QtThread::safeAsync([dialogExists, romsProcessed, &dialog]() {
        if (!*dialogExists)
          return;
        dialog.setValue(romsProcessed);
      });

      processRom(rom, romInfoTable, romFiles, fixEncoding);
      romsProcessed++;
    }

    QtThread::safeAsync([dialogExists, &dialog]() {
      if (!*dialogExists)
        return;
      dialog.setLabelText("Moving rom information to the database...");
      dialog.setRange(0, 0);
      dialog.setValue(0);
    });

    SqlTransaction transaction;
    foreach_cvalue(romInfo, romInfoTable) { DataProvider::addRomInfo(romInfo); }
    for (const RomFile &romFile : romFiles) {
      DataProvider::addRomFile(romFile);
    }
    transaction.commit();

    QtThread::safeAsync([dialogExists, &migrationFinished, &dialog]() {
      if (!*dialogExists)
        return;
      migrationFinished = true;
      dialog.close();
    });
  });

  dialog.exec();

  *dialogExists = false;
  migrationWorker.join();

  fs::error_code err;
  fs::remove(filePath, err);
}

static inline void migrateGroupsToSqlite() {
  const fs::path filePath = BaseDir::data() / _NFS("groups.json");
  if (!fs::isRegularFileSafe(filePath))
    return;

  InputFile groupsFile(filePath, false);
  const Json groupListJson = Json::parse(groupsFile);
  groupsFile.close();

  if (!groupListJson.isArray()) {
    fs::error_code err;
    fs::remove(filePath, err);
  }

  const JArray &groups = groupListJson.array();
  if (groups.empty()) {
    fs::error_code err;
    fs::remove(filePath, err);
  }

  SqlTransaction transaction;
  for (const Json &group : groups) {
    const string groupName = group.getOrDefault<string>("");
    if (groupName.empty() || groupName == "Favourites")
      continue;
    DataProvider::addGroup(groupName);
  }
  DataProvider::addGroup(SpecialGroups::Favourites);
  transaction.commit();

  fs::error_code err;
  fs::remove(filePath, err);
}

static inline void migrateManualRomPaths() {
  const fs::path filePath = BaseDir::data() / _NFS("sources2.json");
  if (!fs::isRegularFileSafe(filePath))
    return;

  InputFile pathsFile(filePath, false);
  const Json pathListJson = Json::parse(pathsFile);
  pathsFile.close();

  if (!pathListJson.isArray()) {
    fs::error_code err;
    fs::remove(filePath, err);
  }

  const JArray &romPaths = pathListJson.array();
  if (romPaths.empty()) {
    fs::error_code err;
    fs::remove(filePath, err);
  }

  SqlTransaction transaction;
  for (const Json &romPath : romPaths) {
    const fs::path path = fs::to_path(romPath.getOrDefault<string>(""));
    if (path.empty() || !fs::existsSafe(path))
      continue;
    DataProvider::addManualRomPath(path);
  }
  transaction.commit();

  fs::error_code err;
  fs::remove(filePath, err);
}

static inline void removeNullTerminators() {
  SqlBatch(R"#(
UPDATE ROMS SET
sha1 = substr( sha1, 0 ),
name = substr( name, 0 ),
internal_name = substr( internal_name, 0 );

UPDATE ROM_PATHS SET
path = substr( path, 0 ),
sha1 = substr( sha1, 0 );

UPDATE ROM_GROUPS SET
sha1 = substr( sha1, 0 ),
group_name = substr( group_name, 0 );

UPDATE PERSISTENT_GROUPS SET
name = substr( name, 0 );

UPDATE MANUAL_ROM_PATHS SET
path = substr( path, 0 );
)#")
      .execNonQuery();
}

static inline void clearRomNameField() {
  SqlCommand cmd(R"#(
UPDATE ROMS
SET name = ?1
)#");
  cmd.addParameter("", DATA_COPY);
  cmd.execNonQuery();
}

static inline void cleanupOrphanedData() {
  SqlCommand(R"#(
DELETE FROM ROM_GROUPS
WHERE sha1 NOT IN( SELECT sha1 FROM ROMS )
)#")
      .execNonQuery();
}

static inline void fixMissingInternalRomNames() {
  SqlCommand(R"#(
UPDATE ROM_PATHS
SET last_modified = 0
WHERE local = 1
AND sha1 IN (
	SELECT sha1 FROM ROMS
	WHERE internal_name = ''
	OR crc32 = 0
)
)#")
      .execNonQuery();
}

static inline void clearBadNameOverrides() {
  SqlCommand(R"#(
UPDATE ROMS
SET name = ''
WHERE sha1 IN (
	SELECT sha1 FROM ROM_PATHS
	INNER JOIN ROMS USING( sha1 )
		WHERE local = 1
		AND name != ''
		AND like( printf( '%%/%s.z64', name ), path )
	UNION
	SELECT sha1 FROM ROMS
	INNER JOIN RHDC_HACK_VERSIONS USING( sha1 )
	INNER JOIN RHDC_HACKS USING( id )
		WHERE ROMS.name = RHDC_HACKS.name
)
)#")
      .execNonQuery();
}

static inline void fixBadMupenPlugins() {
  SqlCommand(R"#(
UPDATE ROMS
SET mupen_plugin = 4
WHERE mupen_plugin = 2
OR mupen_plugin =  5
)#")
      .execNonQuery();
}

static inline void migrateHotkeys() {
  std::vector<KeyId> hotkeys = FileController::loadHotkeys();

  if (hotkeys.size() != (size_t)Hotkey::NUM_HOTKEYS) {
    hotkeys = std::vector<KeyId>(Hotkeys::Default,
                                 Hotkeys::Default + (int)Hotkey::NUM_HOTKEYS);
    FileController::saveHotkeys(hotkeys);
    return;
  }

  bool defaultInitialized = true;
  for (size_t i = 0; i < hotkeys.size(); i++) {
    if (hotkeys[i] != Hotkeys::Default[i]) {
      defaultInitialized = false;
      break;
    }
  }

  if (defaultInitialized)
    return;

  for (KeyId &key : hotkeys) {
    switch ((int)key) {
    case Qt::Key_QuoteDbl:
    case Qt::Key_Apostrophe:
      key = KeyId::Space;
      break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
      key = KeyId::Equals;
      break;
    case Qt::Key_Less:
    case Qt::Key_Comma:
      key = KeyId::Comma;
      break;
    case Qt::Key_Underscore:
    case Qt::Key_Minus:
      key = KeyId::Hyphen;
      break;
    case Qt::Key_Greater:
    case Qt::Key_Period:
      key = KeyId::Period;
      break;
    case Qt::Key_Question:
    case Qt::Key_Slash:
      key = KeyId::Slash;
      break;
    case Qt::Key_Colon:
    case Qt::Key_Semicolon:
      key = KeyId::Semicolon;
      break;
    case Qt::Key_AsciiTilde:
    case Qt::Key_QuoteLeft:
      key = KeyId::Grave;
      break;
    case Qt::Key_Print:
    case Qt::Key_SysReq:
      key = KeyId::PrintScreen;
      break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      key = KeyId::Enter;
      break;
    case Qt::KeypadModifier | Qt::Key_Return:
    case Qt::KeypadModifier | Qt::Key_Enter:
      key = KeyId::NumpadEnter;
      break;
    case Qt::Key_Space:
      key = KeyId::Space;
      break;
    case Qt::Key_0:
      key = KeyId::Num0;
      break;
    case Qt::Key_1:
      key = KeyId::Num1;
      break;
    case Qt::Key_2:
      key = KeyId::Num2;
      break;
    case Qt::Key_3:
      key = KeyId::Num3;
      break;
    case Qt::Key_4:
      key = KeyId::Num4;
      break;
    case Qt::Key_5:
      key = KeyId::Num5;
      break;
    case Qt::Key_6:
      key = KeyId::Num6;
      break;
    case Qt::Key_7:
      key = KeyId::Num7;
      break;
    case Qt::Key_8:
      key = KeyId::Num8;
      break;
    case Qt::Key_9:
      key = KeyId::Num9;
      break;
    case Qt::Key_A:
      key = KeyId::A;
      break;
    case Qt::Key_B:
      key = KeyId::B;
      break;
    case Qt::Key_C:
      key = KeyId::C;
      break;
    case Qt::Key_D:
      key = KeyId::D;
      break;
    case Qt::Key_E:
      key = KeyId::E;
      break;
    case Qt::Key_F:
      key = KeyId::F;
      break;
    case Qt::Key_G:
      key = KeyId::G;
      break;
    case Qt::Key_H:
      key = KeyId::H;
      break;
    case Qt::Key_I:
      key = KeyId::I;
      break;
    case Qt::Key_J:
      key = KeyId::J;
      break;
    case Qt::Key_K:
      key = KeyId::K;
      break;
    case Qt::Key_L:
      key = KeyId::L;
      break;
    case Qt::Key_M:
      key = KeyId::M;
      break;
    case Qt::Key_N:
      key = KeyId::N;
      break;
    case Qt::Key_O:
      key = KeyId::O;
      break;
    case Qt::Key_P:
      key = KeyId::P;
      break;
    case Qt::Key_Q:
      key = KeyId::Q;
      break;
    case Qt::Key_R:
      key = KeyId::R;
      break;
    case Qt::Key_S:
      key = KeyId::S;
      break;
    case Qt::Key_T:
      key = KeyId::T;
      break;
    case Qt::Key_U:
      key = KeyId::U;
      break;
    case Qt::Key_V:
      key = KeyId::V;
      break;
    case Qt::Key_W:
      key = KeyId::W;
      break;
    case Qt::Key_X:
      key = KeyId::X;
      break;
    case Qt::Key_Y:
      key = KeyId::Y;
      break;
    case Qt::Key_Z:
      key = KeyId::Z;
      break;
    case Qt::Key_BracketLeft:
      key = KeyId::LeftBracket;
      break;
    case Qt::Key_Backslash:
      key = KeyId::Backslash;
      break;
    case Qt::Key_BracketRight:
      key = KeyId::RightBracket;
      break;
    case Qt::Key_Escape:
      key = KeyId::Escape;
      break;
    case Qt::Key_Tab:
      key = KeyId::Tab;
      break;
    case Qt::Key_Backspace:
      key = KeyId::Backspace;
      break;
    case Qt::Key_Insert:
      key = KeyId::Insert;
      break;
    case Qt::Key_Delete:
      key = KeyId::Delete;
      break;
    case Qt::Key_Pause:
      key = KeyId::Break;
      break;
    case Qt::Key_Home:
      key = KeyId::Home;
      break;
    case Qt::Key_End:
      key = KeyId::End;
      break;
    case Qt::Key_Left:
      key = KeyId::ArrowLeft;
      break;
    case Qt::Key_Up:
      key = KeyId::ArrowUp;
      break;
    case Qt::Key_Right:
      key = KeyId::ArrowRight;
      break;
    case Qt::Key_Down:
      key = KeyId::ArrowDown;
      break;
    case Qt::Key_PageUp:
      key = KeyId::PageUp;
      break;
    case Qt::Key_PageDown:
      key = KeyId::PageDown;
      break;
    case Qt::Key_Shift:
      key = KeyId::LeftShift;
      break;
    case Qt::Key_Control:
      key = KeyId::LeftControl;
      break;
    case Qt::Key_CapsLock:
      key = KeyId::CapsLock;
      break;
    case Qt::Key_NumLock:
      key = KeyId::NumLock;
      break;
    case Qt::Key_ScrollLock:
      key = KeyId::ScrollLock;
      break;
    case Qt::Key_F1:
      key = KeyId::F1;
      break;
    case Qt::Key_F2:
      key = KeyId::F2;
      break;
    case Qt::Key_F3:
      key = KeyId::F3;
      break;
    case Qt::Key_F4:
      key = KeyId::F4;
      break;
    case Qt::Key_F5:
      key = KeyId::F5;
      break;
    case Qt::Key_F6:
      key = KeyId::F6;
      break;
    case Qt::Key_F7:
      key = KeyId::F7;
      break;
    case Qt::Key_F8:
      key = KeyId::F8;
      break;
    case Qt::Key_F9:
      key = KeyId::F9;
      break;
    case Qt::Key_F10:
      key = KeyId::F10;
      break;
    case Qt::Key_F11:
      key = KeyId::F11;
      break;
    case Qt::Key_F12:
      key = KeyId::F12;
      break;
    case Qt::Key_F13:
      key = KeyId::F13;
      break;
    case Qt::Key_F14:
      key = KeyId::F14;
      break;
    case Qt::Key_F15:
      key = KeyId::F15;
      break;
    case Qt::Key_F16:
      key = KeyId::F16;
      break;
    case Qt::Key_F17:
      key = KeyId::F17;
      break;
    case Qt::Key_F18:
      key = KeyId::F18;
      break;
    case Qt::Key_F19:
      key = KeyId::F19;
      break;
    case Qt::Key_F20:
      key = KeyId::F20;
      break;
    case Qt::Key_F21:
      key = KeyId::F21;
      break;
    case Qt::Key_F22:
      key = KeyId::F22;
      break;
    case Qt::Key_F23:
      key = KeyId::F23;
      break;
    case Qt::Key_F24:
      key = KeyId::F24;
      break;
    case Qt::KeypadModifier | Qt::Key_Asterisk:
      key = KeyId::NumpadAsterisk;
      break;
    case Qt::KeypadModifier | Qt::Key_Plus:
      key = KeyId::NumpadPlus;
      break;
    case Qt::KeypadModifier | Qt::Key_Minus:
      key = KeyId::NumpadMinus;
      break;
    case Qt::KeypadModifier | Qt::Key_Period:
      key = KeyId::NumpadPeriod;
      break;
    case Qt::KeypadModifier | Qt::Key_Slash:
      key = KeyId::NumpadSlash;
      break;
    case Qt::KeypadModifier | Qt::Key_0:
      key = KeyId::Numpad0;
      break;
    case Qt::KeypadModifier | Qt::Key_1:
      key = KeyId::Numpad1;
      break;
    case Qt::KeypadModifier | Qt::Key_2:
      key = KeyId::Numpad2;
      break;
    case Qt::KeypadModifier | Qt::Key_3:
      key = KeyId::Numpad3;
      break;
    case Qt::KeypadModifier | Qt::Key_4:
      key = KeyId::Numpad4;
      break;
    case Qt::KeypadModifier | Qt::Key_5:
      key = KeyId::Numpad5;
      break;
    case Qt::KeypadModifier | Qt::Key_6:
      key = KeyId::Numpad6;
      break;
    case Qt::KeypadModifier | Qt::Key_7:
      key = KeyId::Numpad7;
      break;
    case Qt::KeypadModifier | Qt::Key_8:
      key = KeyId::Numpad8;
      break;
    case Qt::KeypadModifier | Qt::Key_9:
      key = KeyId::Numpad9;
      break;
    default:
      key = KeyId::INVALID;
      break;
    }
  }
  FileController::saveHotkeys(hotkeys);
}

static inline void clearLayouts() {
  fs::error_code err;
  fs::remove_all(BaseDir::data() / _NFS("jsml"), err);
  fs::remove_all(BaseDir::data() / _NFS("layouts"), err);
}

static inline void fixUpperCaseChecksums() {
  SqlBatch(R"#(
UPDATE RHDC_LAST_PLAYED_VERSIONS
SET sha1 = lower( sha1 )
WHERE sha1 != lower( sha1 );

UPDATE OR IGNORE ROMS
SET sha1 = lower( sha1 )
WHERE sha1 != lower( sha1 );

DELETE FROM ROMS
WHERE sha1 != lower( sha1 );

UPDATE OR IGNORE RHDC_HACK_VERSIONS
SET sha1 = lower( sha1 )
WHERE sha1 != lower( sha1 );

DELETE FROM RHDC_HACK_VERSIONS
WHERE sha1 != lower( sha1 );
)#")
      .execNonQuery();
}

static inline void dirtyNonAsciiPaths() {
  std::vector<string> affectedPaths;

  SqlCommand query("SELECT path FROM ROM_PATHS");
  DataRecordIterator paths = query.execQuery();
  while (paths.moveNext()) {
    const string path = paths.getString(0);
    for (char c : path) {
      if ((ubyte)c > 0x7f) {
        affectedPaths.push_back(path);
      }
    }
  }

  SqlCommand dirtyCmd(R"#(
UPDATE ROM_PATHS
SET last_modified = 0
WHERE path = ?1
)#");

  for (const string &path : affectedPaths) {
    dirtyCmd.addParameter(path, DATA_REF);
    dirtyCmd.execNonQuery();
  }
}

static inline void fixGlnPluginUsage(AppSettings &settings) {
  if (settings.defaultParallelPlugin == GfxPlugin::GLideN64) {
    settings.defaultParallelPlugin = GfxPlugin::Gln64;
  }

  SqlCommand cmd(R"#(
UPDATE ROMS
SET parallel_plugin = 6
WHERE parallel_plugin = 4
)#");
  cmd.execNonQuery();
}

static inline void migrateInputFiles() {
#ifdef __APPLE__
#define INPUT_DRIVER_SUFFIX "-hid"
#else
#define INPUT_DRIVER_SUFFIX "-sdl"
#endif
  fs::error_code err;

  if (fs::exists(BaseDir::data() / _NFS("profiles.json"), err)) {
    fs::rename(BaseDir::data() / _NFS("profiles.json"),
               BaseDir::data() / (_NFS("profiles") INPUT_DRIVER_SUFFIX ".json"),
               err);
  }

  if (fs::exists(BaseDir::data() / _NFS("devices.json"), err)) {
    fs::rename(BaseDir::data() / _NFS("devices.json"),
               BaseDir::data() / (_NFS("devices") INPUT_DRIVER_SUFFIX ".json"),
               err);
  }

  if (fs::exists(BaseDir::cache() / _NFS("last_profile.json"), err)) {
    fs::rename(BaseDir::cache() / _NFS("last_profile.json"),
               BaseDir::cache() /
                   (_NFS("last_profile") INPUT_DRIVER_SUFFIX ".json"),
               err);
  }

#undef INPUT_DRIVER_SUFFIX
}

static inline void updateEmulatorDefaults(AppSettings &settings) {
  settings.defaultEmulator = EmulatorCore::ParallelN64;
  if (settings.defaultParallelPlugin == GfxPlugin::Glide64 ||
      settings.defaultParallelPlugin == GfxPlugin::Rice) {
    settings.defaultParallelPlugin = GfxPlugin::OGRE;
  }

#ifdef __APPLE__
  settings.glidenAntiAliasing = false;
  if (settings.defaultParallelPlugin == GfxPlugin::ParaLLEl) {
    settings.defaultParallelPlugin = GfxPlugin::GLideN64;
  }

  RhdcSettings rhdcSettings = FileController::loadRhdcSettings();
  rhdcSettings.preferHle = true;
  FileController::saveRhdcSettings(rhdcSettings);
#endif
}

static inline void migrateHackPlugins() {
  SqlBatch cmd(R"#(
UPDATE ROMS SET
	parallel_plugin = 7,
	widescreen = 0
WHERE (parallel_plugin = 2 OR parallel_plugin = 5)
AND sha1 <> "6fd17e6c2116925550765086675c61ae62fd8761"
AND sha1 <> "5043ced06d13e87c2d0a8b4ab1e44763c317aba6";

UPDATE ROMS SET
	parallel_plugin = 2,
	emulate_framebuffer = 0,
	n64_depth_compare = 0
WHERE parallel_plugin = 6;
)#");
  cmd.execNonQuery();
}

#ifdef __APPLE__
static inline void avoidParallelOnMacOS() {
  SqlCommand cmd(R"#(
UPDATE ROMS SET
	parallel_plugin = 4,
	emulate_framebuffer = 1
WHERE parallel_plugin = 1
)#");
  cmd.execNonQuery();
}

static inline void uninstallRetroArchMacOS() {
  fs::error_code err;
  fs::remove(RetroArch::getExePath(), err);

  InstalledVersionsInfo versions = FileController::loadInstalledVersions();
  versions.retroArchVersion.version = Version{0, 0, 0};
  FileController::saveInstalledVersions(versions);
}
#endif

static inline void removeParallelBordersForRhdcRoms() {
  SqlCommand cmd(R"#(
UPDATE ROMS
	SET remove_borders = 1
WHERE internal_name = 'SUPER MARIO 64'
OR sha1 IN ( SELECT sha1 FROM RHDC_HACK_VERSIONS )
)#");
  cmd.execNonQuery();
}

static inline void getRomInputTypes() {
  for (const RomFile &rom : DataProvider::fetchLocalRomFiles()) {
    if (!rom.local || !fs::existsSafe(rom.path))
      continue;
    SqlCommand cmd(R"#(
UPDATE ROMS
	SET input_types = ?2
WHERE sha1 = ?1
)#");
    cmd.addParameter(rom.sha1, DATA_REF);
    cmd.addParameter(RomUtil::readControllerTypes(rom.path), DATA_COPY);
    cmd.execNonQuery();
  }
}

static inline void renameParallelCore() {
#if defined(_WIN32)
#define PC_LIB_EXT L".dll"
#elif defined(__APPLE__)
#define PC_LIB_EXT ".dylib"
#else
#define PC_LIB_EXT ".so"
#endif
  const fs::path coreDir = BaseDir::data() / _NFS("retro-data") / _NFS("cores");
  if (fs::exists(coreDir / (_NFS("parallel_n64_libretro") PC_LIB_EXT))) {
    fs::error_code err;
    fs::rename(coreDir / (_NFS("parallel_n64_libretro") PC_LIB_EXT),
               coreDir / (_NFS("parallel_n64_next_libretro") PC_LIB_EXT), err);
  }

#undef PC_LIB_EXT
}

static inline void removePatchesAsRoms() {
  // Deletes "roms" that are actually patch files
  SqlBatch(R"#(
DELETE FROM ROM_GROUPS
WHERE sha1 IN(
	SELECT DISTINCT sha1 FROM ROM_PATHS
	WHERE path LIKE '%.bps'
	AND local = 1
);

DELETE FROM ROMS
WHERE sha1 IN(
	SELECT DISTINCT sha1 FROM ROM_PATHS
	WHERE path LIKE '%.bps'
	AND local = 1
);

DELETE FROM ROM_PATHS
WHERE path LIKE '%.bps'
AND local = 1;

DELETE FROM MANUAL_ROM_PATHS
WHERE path LIKE '%.bps'
)#")
      .execNonQuery();
}

static inline void queuePluginChecks() {
  SqlCommand cmd(R"#(
INSERT OR IGNORE INTO PENDING_PLUGIN_CHECKS
SELECT DISTINCT sha1 FROM ROMS
EXCEPT SELECT DISTINCT sha1 FROM RHDC_HACK_VERSIONS
)#");
  cmd.execNonQuery();
}

static inline void resetObviouslyIncorrectPlayTimes() {
  SqlCommand cmd(R"#(
UPDATE ROMS
SET play_time = 0
WHERE play_time > 5400000000
)#");
  cmd.execNonQuery();
}

static inline void syncVersionPlayTimes() {
  SqlCommand cmd(R"#(
WITH synced_play_times AS (
	SELECT id, MAX( play_time ) AS play_time FROM ROMS
	INNER JOIN RHDC_HACK_VERSIONS USING( sha1 )
	GROUP BY id
)
UPDATE ROMS
SET play_time = synced_play_times.play_time
FROM synced_play_times
INNER JOIN RHDC_HACK_VERSIONS USING( id )
WHERE ROMS.sha1 = RHDC_HACK_VERSIONS.sha1
)#");
  cmd.execNonQuery();
}

static inline void fixMisencodedRomNames() {
  SqlCommand cmd1(
      "UPDATE ROMS "
      "SET internal_name = REPLACE( internal_name, '0\x99', '\xE3\x82\x99' ) "
      "WHERE internal_name LIKE '%0\x99%'");
  cmd1.execNonQuery();

  SqlCommand cmd2(
      "UPDATE ROMS "
      "SET internal_name = REPLACE( internal_name, '0\x99', '\xE3\x82\x9A' ) "
      "WHERE internal_name LIKE '%0\x9A%'");
  cmd2.execNonQuery();
}

static inline void recheckInputTypes() {
  SqlCommand cmd(R"#(
UPDATE ROMS
SET input_types = x'DEADBEEF'
WHERE input_types = x'00000000'
)#");
  cmd.execNonQuery();
}

static inline bool isEmptyDirectory(const fs::path &path) {
  fs::error_code err;
  auto i = fs::directory_iterator(path, err);
  return !err && fs::begin(i) == fs::end(i);
}

static inline void moveSavesUp(const fs::path &path) {
  fs::error_code err;
  if (!fs::is_directory(path, err) || err)
    return;

  const fs::path &root = path.parent_path();
  for (const fs::directory_entry &node : fs::directory_iterator(path, err)) {
    err.clear();
#ifdef __APPLE__
    if (node.status(err).type() != fs::regular_file || err)
      continue;
#else
    if (!node.is_regular_file(err) || err)
      continue;
#endif

    const fs::path target = root / node.path().filename();
    if (fs::exists(target, err) || err)
      continue;

    fs::rename(node.path(), target, err);
  }

  err.clear();
  if (isEmptyDirectory(path))
    fs::remove(path, err);
}

static inline void fixMisplacedSaves() {
  fs::error_code err;

  fs::path saveDir = RetroArch::getBasePath() / _NFS("saves");
  if (!fs::is_directory(saveDir, err) || err)
    return;
  for (const fs::directory_entry &node : fs::directory_iterator(saveDir, err)) {
    err.clear();
#ifdef __APPLE__
    if (node.status(err).type() != fs::directory_file || err)
      continue;
#else
    if (!node.is_directory(err) || err)
      continue;
#endif
    if (node.path().filename().u8string() == "sync")
      continue;
    moveSavesUp(node.path());
  }

  err.clear();
  saveDir = saveDir / _NFS("sync");
  if (!fs::is_directory(saveDir, err) || err)
    return;
  for ([[maybe_unused]] const fs::directory_entry &node :
       fs::directory_iterator(saveDir, err)) {
    err.clear();
#ifdef __APPLE__
    if (node.status(err).type() != fs::directory_file || err)
      continue;
#else
    if (!node.is_directory(err) || err)
      continue;
#endif
    moveSavesUp(node.path());
  }
}

static inline void migrateRhdcRatings() {
  SqlCommand cmd(R"#(
UPDATE RHDC_HACK_PROGRESS
	SET my_rating = CASE
		WHEN my_rating > 0 AND my_rating <= 3 THEN 1
		WHEN my_rating > 3 AND my_rating <= 6 THEN 2
		WHEN my_rating > 6 AND my_rating <= 8 THEN 3
		WHEN my_rating > 8 THEN 4
		ELSE 0
	END
)#");
  cmd.execNonQuery();
}

static inline void fixInvalidUnicode() {
  SqlCommand cmd("SELECT sha1, internal_name FROM ROMS");
  DataRecordIterator results = cmd.execQuery();

  std::vector<std::pair<string, string>> changes;
  string safeString;
  while (results.moveNext()) {
    if (!Utf8::validate(results.getString(1), &safeString)) {
      std::cout << safeString << std::endl;
      changes.push_back({results.getString(0), safeString});
    }
  }

  if (changes.empty())
    return;

  SqlBatch update(R"#(
		UPDATE ROMS
			SET internal_name = ?2
		WHERE sha1 = ?1;

		UPDATE ROM_PATHS
			SET last_modified = 0
		WHERE sha1 = ?1;
)#");

  SqlTransaction transaction;
  for (const auto &chg : changes) {
    update.addParameter(chg.first, DATA_REF);
    update.addParameter(chg.second, DATA_REF);
    update.execNonQuery();
  }
  transaction.commit();
}

void Migration::run() {
  AppSettings settings = FileController::loadAppSettings();

  if (settings.migrationState < 2) {
    RetroArch::migrateSaves();
  }

  if (settings.migrationState < 3) {
    migrateRomsToSqlite(settings.migrationState < 1);
    migrateGroupsToSqlite();
    migrateManualRomPaths();
  }

  if (settings.migrationState < 4) {
    removeNullTerminators();
    clearRomNameField();
  }

  if (settings.migrationState < 5) {
    cleanupOrphanedData();
  }

  if (settings.migrationState < 6) {
    settings.glidenAntiAliasing = true;
  }

  if (settings.migrationState < 8) {
    fixMissingInternalRomNames();
    clearBadNameOverrides();
  }

  if (settings.migrationState < 9) {
    fixBadMupenPlugins();
  }

  if (settings.migrationState < 10) {
    migrateHotkeys();
  }

  if (settings.migrationState < 13) {
    clearLayouts();
  }

#ifdef _WIN32
  if (settings.migrationState < 15) {
    SqlCommand cmd(R"#(
SELECT path, internal_name FROM
ROM_PATHS INNER JOIN ROMS USING( sha1 )
	WHERE local = 1
	AND internal_name != ''
)#");

    DataRecordIterator iterator = cmd.execQuery();
    while (iterator.moveNext()) {
      SaveConverter::autoImportFromProject64(
          RetroArch::getSaveFilePath(iterator.getPath(0)),
          iterator.getString(1));
    }
  }
#endif

  if (settings.migrationState < 16) {
    fixUpperCaseChecksums();
  }

#ifdef _WIN32
  if (settings.migrationState < 17) {
    dirtyNonAsciiPaths();
  }
#endif

  if (settings.migrationState < 18) {
    fixGlnPluginUsage(settings);
  }

  if (settings.migrationState < 19) {
    migrateInputFiles();
#ifdef __APPLE__
    const ControllerProfile &profile = DefaultProfile::get(InputDriver::HID);
    if (settings.migrationState < 11) {
      FileController::saveControllerProfiles(InputDriver::HID,
                                             {{profile.name, profile}});
    }
#endif
  }

  if (settings.migrationState < 22) {
    updateEmulatorDefaults(settings);
    migrateHackPlugins();
#ifdef __APPLE__
    avoidParallelOnMacOS();
#endif
#ifdef __APPLE__
    uninstallRetroArchMacOS();
#endif
  }

  if (settings.migrationState < 23) {
    removeParallelBordersForRhdcRoms();
  }

  if (settings.migrationState < 24) {
    getRomInputTypes();
  }

  if (settings.migrationState < 25) {
#ifdef FLATPAK_VERSION
    settings.discordIntegration = true;
#endif
    renameParallelCore();
  }

  if (settings.migrationState < 26) {
    DataProvider::deleteGroup("\x01"
                              "80");
    removePatchesAsRoms();
  }

  if (settings.migrationState < 27) {
    queuePluginChecks();
  }

#ifdef __APPLE__
  if (settings.migrationState < 28) {
    RhdcSettings rhdcSettings = FileController::loadRhdcSettings();
    rhdcSettings.preferHle = false;
    FileController::saveRhdcSettings(rhdcSettings);
  }
#endif

  if (settings.migrationState < 29) {
    settings.vsync = true;
  }

#ifdef _WIN32
  if (settings.migrationState < 30) {
    settings.vsync = false;
  }
#endif

  if (settings.migrationState < 31) {
    resetObviouslyIncorrectPlayTimes();
  }

  if (settings.migrationState < 32) {
    if (fs::isDirectorySafe(BaseDir::data() / _NFS("thumbnails"))) {
      fs::error_code err;
      fs::rename(BaseDir::data() / _NFS("thumbnails"),
                 BaseDir::cache() / _NFS("thumbnails"), err);
    }
  }

  if (settings.migrationState < 33) {
    fixMisencodedRomNames();
  }

  if (settings.migrationState < 34) {
    syncVersionPlayTimes();
    recheckInputTypes();
    fixMisplacedSaves();
  }

#ifdef __APPLE__
  if (settings.migrationState < 35) {
    settings.forceX86 = false;
  }
#endif

#if defined(__linux__) && !defined(FLATPAK_VERSION)
  if (settings.migrationState < 36) {
    fs::error_code err;
    if (fs::is_directory(BaseDir::data() / _NFS("retroarch"), err) && !err) {
      constexpr char s_text[] =
          "Parallel Launcher now manages RetroArch by downloading appimages "
          "instead of using Flatpak.\n\n"
          "Your Flatpak installation of RetroArch that was created by an older "
          "version of Parallel Launcher "
          "can now be safely deleted to free up disk space (~3 GiB).\n\nWould "
          "you like to do that now?";

      bool deleted = false;
      if (QMessageBox::question(nullptr, "RetroArch Cleanup", s_text) ==
          QMessageBox::Yes) {
        deleted =
            !AsyncProcess("pkexec",
                          {"rm", "-rf",
                           (BaseDir::data() / "retroarch").u8string().c_str()})
                 .join();
      }

      const string message =
          ("Your old RetroArch flatpak installation was not deleted.\n"
           "If you want to delete it later, you can find it at\n") +
          (BaseDir::data() / "retroarch").u8string();

      if (!deleted) {
        QMessageBox::warning(nullptr, "Cleanup Cancelled", message.c_str());
      }
    }
  }
#endif

  if (settings.migrationState < 37) {
    settings.glidenAntiAliasing = false;
  }

  if (settings.migrationState < 38) {
    migrateRhdcRatings();
  }

  if (settings.migrationState < 39) {
    fixInvalidUnicode();
  }

  settings.migrationState = 39;
  FileController::saveAppSettings(settings);
}

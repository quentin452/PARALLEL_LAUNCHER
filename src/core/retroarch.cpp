#include "src/core/retroarch.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/hotkeys.hpp"
#include "src/core/logging.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/rom.hpp"
#include "src/core/steam-deck.hpp"
#include "src/db/data-provider.hpp"
#include "src/input/gamepad-controller.hpp"
#include "src/input/input-driver.hpp"
#include "src/input/keyboard.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/crash.hpp"
#include "src/polyfill/env.hpp"
#include "src/polyfill/file.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/ui/screen.hpp"
#include <QApplication>
#include <QCoreApplication>
#include <QScreen>
#include <clocale>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <regex>
#include <unordered_map>

#if defined(__linux__) && !defined(FLATPAK_VERSION)
#include "src/input/common/sdl-driver.hpp"
#endif

#ifdef __APPLE__
#include "src/polyfill/macos/apple-util.hpp"
#endif

static fs::path expand_path(const fs::path &path) {
  const string pathStr = path.u8string();
  if (pathStr == ":") {
    return RetroArch::getBasePath();
  } else if (pathStr.length() >= 2 && pathStr[0] == ':' &&
             (pathStr[1] == '/' || pathStr[1] == '\\')) {
#ifdef _WIN32
    return RetroArch::getBasePath() / fs::path(&path.wstring().c_str()[2]);
#else
    return RetroArch::getBasePath() / fs::path(&path.u8string().c_str()[2]);
#endif
  } else
    return BaseDir::expandHome(path);
}

static const char *s_plugins[] = {
    "auto",    "parallel", "glide64", "angrylion", "gliden64", "rice", "gln64",
    "gliden64" // OGRE is just GLideN64 with a feature flag set
};

ubyte RetroArch::resolveUpscaling(ParallelUpscaling requestedUpscaling,
                                  ubyte windowScale) noexcept {
  if (requestedUpscaling != ParallelUpscaling::Auto)
    return 1 << ((ubyte)requestedUpscaling - 1);
  if (windowScale <= 1)
    return 1;
  if (windowScale == 2)
    return 2;
  if (windowScale < 8)
    return 4;
  return 8;
}

static const std::regex s_cfgRegex("^([^=]+)\\s*=\\s*\"([^\"]*)\"",
                                   std::regex_constants::ECMAScript |
                                       std::regex_constants::optimize);

static void loadConfigHelper(const fs::path &configPath,
                             std::map<string, string> &configs) {
  if (!fs::existsSafe(configPath))
    return;
  InputFile configFile(configPath, false);
  while (configFile.good() && !configFile.eof()) {
    string line;
    std::getline(configFile, line);

    std::smatch matches;
    if (std::regex_search(line, matches, s_cfgRegex)) {
      string key = matches[1];
      while (!key.empty() && key.back() == ' ')
        key.pop_back();
      if (!key.empty()) {
        configs[key] = matches[2];
      }
    }
  }
}

static void loadConfig(std::map<string, string> &configs) {
  loadConfigHelper(BaseDir::data() / _NFS("retroarch.cfg"), configs);
  loadConfigHelper(BaseDir::data() / _NFS("retroarch-core-options.cfg"),
                   configs);

  const fs::path basePath = RetroArch::getBasePath();
  configs["system_directory"] = (basePath / _NFS("system")).u8string();
  configs["libretro_directory"] = (basePath / _NFS("cores")).u8string();
  configs["savefile_directory"] = (basePath / _NFS("saves")).u8string();
  configs["savestate_directory"] = (basePath / _NFS("states")).u8string();
  configs["libretro_info_path"] = (basePath / _NFS("info")).u8string();
  configs["log_dir"] = basePath.u8string();
#if defined(FLATPAK_VERSION)
  configs["assets_directory"] = "/app/share/libretro/assets/";
#elif defined(__linux__)
  configs["assets_directory"] =
      (BaseDir::data() / "retro-data" / "assets").u8string();
  configs["osk_overlay_directory"] =
      (BaseDir::data() / "retro-data" / "overlays" / "keyboards").u8string();
  configs["overlay_directory"] =
      (BaseDir::data() / "retro-data" / "overlays").u8string();
  configs["audio_filter_dir"] =
      (BaseDir::data() / "retro-data" / "filters" / "audio").u8string();
  configs["video_filter_dir"] =
      (BaseDir::data() / "retro-data" / "filters" / "video").u8string();
  configs["video_shader_dir"] =
      (BaseDir::data() / "retro-data" / "shaders").u8string();
  configs["cheat_database_path"] =
      (BaseDir::data() / "retro-data" / "cheats").u8string();
  configs["recording_config_directory"] =
      (BaseDir::data() / "retro-data" / "records_config").u8string();
  configs["recording_output_directory"] =
      (BaseDir::data() / "retro-data" / "records").u8string();
  configs["screenshot_directory"] =
      (BaseDir::data() / "retro-data" / "screenshots").u8string();
#elif defined(__APPLE__)
  configs["assets_directory"] = (BaseDir::home() / "Library" /
                                 "Application Support" / "RetroArch" / "assets")
                                    .u8string();
#else
  configs["assets_directory"] =
      (BaseDir::data() / L"retroarch" / L"assets").u8string();
#endif
}

static void
saveConfig([[maybe_unused]] const std::map<string, string> &configs) {
  OutputFile mainConfigFile(BaseDir::data() / _NFS("retroarch.cfg"), false);
  OutputFile coreConfigFile(
      BaseDir::data() / _NFS("retroarch-core-options.cfg"), false);

  for (const auto &i : configs) {
    const string &key = i.first;
    const string &value = i.second;

    if (std::strncmp(key.c_str(), "parallel-n64-",
                     sizeof("parallel-n64-") - 1) == 0 ||
        std::strncmp(key.c_str(), "mupen64plus-", sizeof("mupen64plus-") - 1) ==
            0) {
      coreConfigFile << key << " = \"" << value << '"' << std::endl;
    } else {
      mainConfigFile << key << " = \"" << value << '"' << std::endl;
    }
  }

  mainConfigFile << std::endl << std::flush;
  coreConfigFile << std::endl << std::flush;
}

#ifdef LEGACY_MAPPINGS
static const char *s_inputNames[] = {
    "l_y_minus", // AnalogUp
    "l_y_plus",  // AnalogDown
    "l_x_minus", // AnalogLeft
    "l_x_plus",  // AnalogRight
    "r_y_minus", // CUp
    "r_y_plus",  // CDown
    "r_x_minus", // CLeft
    "r_x_plus",  // CRight
    "up",        // DPadUp
    "down",      // DPadDown
    "left",      // DPadLeft
    "right",     // DPadRight
    "b",         // ButtonA
    "y",         // ButtonB
    "l",         // TriggerL
    "l2",        // TriggerZ
    "r",         // TriggerR
    "start",     // Start
    "l3",        // ButtonX
    "r3",        // ButtonY
};

static const char *s_unusedInputs[] = {"a", "x", "r2", "select"};
#else
static const char *s_inputNames[] = {
    "l_y_minus", // AnalogUp
    "l_y_plus",  // AnalogDown
    "l_x_minus", // AnalogLeft
    "l_x_plus",  // AnalogRight
    "x",         // CUp
    "a",         // CDown
    "l",         // CLeft
    "r",         // CRight
    "up",        // DPadUp
    "down",      // DPadDown
    "left",      // DPadLeft
    "right",     // DPadRight
    "b",         // ButtonA
    "y",         // ButtonB
    "select",    // TriggerL
    "l2",        // TriggerZ
    "r2",        // TriggerR
    "start",     // Start
    "l3",        // ButtonX
    "r3",        // ButtonY
};

static const char *s_unusedInputs[] = {"r_y_minus", "r_y_plus", "r_x_minus",
                                       "r_x_plus"};
#endif

static void bindInput(std::map<string, string> &configs, const string &key,
                      const Binding &binding) {
  const string buttonKey = key + "_btn";
  const string axisKey = key + "_axis";

  // 99 and +99 are used instead of nul to prevent default bindings from messing
  // things up
  switch (binding.type) {
  case BindingType::None:
    configs[buttonKey] = "99";
    configs[axisKey] = "+99";
    break;
  case BindingType::Button:
    configs[buttonKey] = Number::toString(binding.buttonOrAxis);
    configs[axisKey] = "+99";
    break;
  case BindingType::AxisNegative:
    configs[buttonKey] = "99";
    configs[axisKey] = "-"s + Number::toString(binding.buttonOrAxis);
    break;
  case BindingType::AxisPositive:
    configs[buttonKey] = "99";
    configs[axisKey] = "+"s + Number::toString(binding.buttonOrAxis);
    break;
  case BindingType::HatUp:
    configs[buttonKey] = "h"s + Number::toString(binding.buttonOrAxis) + "up";
    configs[axisKey] = "+99";
    break;
  case BindingType::HatDown:
    configs[buttonKey] = "h"s + Number::toString(binding.buttonOrAxis) + "down";
    configs[axisKey] = "+99";
    break;
  case BindingType::HatLeft:
    configs[buttonKey] = "h"s + Number::toString(binding.buttonOrAxis) + "left";
    configs[axisKey] = "+99";
    break;
  case BindingType::HatRight:
    configs[buttonKey] =
        "h"s + Number::toString(binding.buttonOrAxis) + "right";
    configs[axisKey] = "+99";
    break;
  }
}

static inline HashMap<Uuid, std::queue<int>> getDevicePorts(int &numMapped) {
  const std::vector<Uuid> uuids =
      GamepadController::instance().getDeviceOrder();
  numMapped = (int)uuids.size();

  HashMap<Uuid, std::queue<int>> devicePorts;
  int i = 0;
  for (const Uuid &uuid : uuids) {
    devicePorts[uuid].push(i++);
  }

  return devicePorts;
}

static inline string getAngrylionFilteringString(AngrylionFiltering filter) {
  switch (filter) {
  case AngrylionFiltering::Filter:
    return "Filtered";
  case AngrylionFiltering::Blur:
    return "AA+Blur";
  case AngrylionFiltering::Dedither:
    return "AA+Dedither";
  case AngrylionFiltering::AntiAlias:
    return "AA only";
  default:
    return "Unfiltered";
  }
}

static inline InputMode getInputMode(const Uuid &id) {
  const std::map<Uuid, InputMode> &inputModes =
      FileController::loadInputModes();
  if (inputModes.count(id) > 0) {
    return inputModes.at(id);
  }
  return DefaultInputModes::Normal;
}

static inline string getMupenWidescreenResolution(ubyte windowScale) {
  switch (windowScale) {
  case 1:
    return "640x360";
  case 2:
    return "960x540";
  case 3:
    return "1280x720";
  case 4:
    return "1920x1080";
  case 5:
    return "2560x1440";
  case 6:
    return "2560x1440";
  case 7:
    return "3840x2160";
  case 8:
    return "3840x2160";
  case 9:
    return "3840x2160";
  default:
    return "1920x1080";
  }
}

static inline bool useDeckMode(WidescreenMode widescreenMode,
                               const RomInfo &romInfo) {
  if (!SteamDeck::isSteamDeck() || SteamDeck::inGamingMode() ||
      (widescreenMode != WidescreenMode::Off))
    return false;

  const RhdcSettings &rhdcSettings = FileController::loadRhdcSettings();
  if (!rhdcSettings.enableStarDisplay)
    return false;

  RhdcHack hack;
  const bool isRhdcHack =
      DataProvider::tryFetchRhdcHackByChecksum(romInfo.sha1, &hack);
  if (!isRhdcHack)
    return false;

  StarLayout layout;
  return rhdcSettings.enableDefaultLayouts ||
         StarLayout::tryLoadLayout(hack.info.hackId, layout);
}

static inline bool use1610(WidescreenMode widescreenMode, bool fullscreen) {
  if (widescreenMode != WidescreenMode::ViewportHack) {
    return false;
  }

  if (!fullscreen && !SteamDeck::isSteamDeck()) {
    return false;
  }

  const QSize resolution = QApplication::primaryScreen()->size();
  if (resolution.height() == 0) {
    return SteamDeck::isSteamDeck();
  }

  const double aspectRatio =
      ((double)resolution.width()) / ((double)resolution.height());
  return aspectRatio < 1.66;
}

AsyncProcess RetroArch::launchRom(fs::path romPath, const AppSettings &settings,
                                  const std::vector<PlayerController> &players,
                                  const RomInfo &romInfo, bool bindSavestate,
                                  const string &saveGroup, GfxPlugin *pluginOut,
                                  const string &isViewerFile,
                                  const string &libplInputPipe,
                                  const string &libplOutputPipe,
                                  bool inputBindingError) {
  std::vector<string> args;
  return AsyncProcess(
#ifdef __APPLE__
      launchBinary,
#else
      RetroArch::getExePath().u8string(),
#endif
      args);
}

fs::path RetroArch::getSaveFilePath(const fs::path &romPath) {
  string groupId;
  if (DataProvider::tryGetSaveGroupForRomPath(romPath, groupId)) {
    return RetroArch::getGroupSaveFilePath(groupId);
  } else {
    return RetroArch::getIndividualSaveFilePath(romPath);
  }
}

fs::path RetroArch::getIndividualSaveFilePath(const fs::path &romPath) {
  fs::error_code err;
  const fs::path saveDir = RetroArch::getBasePath() / _NFS("saves");
  fs::create_directories(saveDir, err);
  fs::path saveFilePath = expand_path(saveDir) / romPath.filename();
  saveFilePath.replace_extension(".srm");
  return saveFilePath;
}

fs::path RetroArch::getGroupSaveFilePath(const string &groupId) {
  fs::error_code err;
  const fs::path saveDir =
      RetroArch::getBasePath() / _NFS("saves") / _NFS("sync");
  fs::create_directories(saveDir, err);
  fs::path saveFilePath = expand_path(saveDir) / fs::to_path(groupId + ".srm");
  return saveFilePath;
}

bool RetroArch::isRetroArchInstalled() {
#ifdef FLATPAK_VERSION
  return true;
#else
  return fs::existsSafe(RetroArch::getExePath());
#endif
}

static bool isCoreInstalled(const char *core) {
  fs::path corePath = RetroArch::getCorePath();
  if (corePath.empty())
    return false;

  corePath = corePath / core;
#if defined(_WIN32)
  corePath += L".dll";
#elif defined(__APPLE__)
  corePath += ".dylib";
#else
  corePath += ".so";
#endif

#ifdef __APPLE__
  return fs::isRegularFileSafe(corePath) &&
         (AppleUtil::shouldUseArmCore() ==
          AppleUtil::isArmLibrary(corePath.u8string().c_str()));
#else
  return fs::isRegularFileSafe(corePath);
#endif
}

bool RetroArch::isEmulatorCoreInstalled(EmulatorCore core) {
  switch (core) {
  case EmulatorCore::ParallelN64:
    return isCoreInstalled("parallel_n64_next_libretro");
  case EmulatorCore::Mupen64plusNext:
    return isCoreInstalled("mupen64plus_next_libretro");
  default:
    return false;
  }
}

static const std::regex
    s_versionRegex("^RetroArch.*v(\\d+)\\.(\\d+)(?:\\.\\d+)*(?:\\s|$)",
                   std::regex_constants::ECMAScript |
                       std::regex_constants::optimize |
                       std::regex_constants::icase);

bool RetroArch::resetConfig() {
  const fs::path mainConfigPath = BaseDir::data() / _NFS("retroarch.cfg");
  const fs::path coreConfigPath =
      BaseDir::data() / _NFS("retroarch-core-options.cfg");

  fs::error_code err;
  if (fs::existsSafe(mainConfigPath))
    fs::remove(mainConfigPath, err);
  if (fs::existsSafe(coreConfigPath))
    fs::remove(coreConfigPath, err);
  return !err;
}

static fs::path createBasePath() {
  const fs::path basePath = BaseDir::data() / _NFS("retro-data");
  fs::create_directories(basePath);
  fs::create_directories(basePath / _NFS("saves"));
  fs::create_directories(basePath / _NFS("states"));
  fs::create_directories(basePath / _NFS("cores"));
  return basePath;
}

fs::path RetroArch::getBasePath() {
  static const fs::path s_path = createBasePath();
  return s_path;
}

fs::path RetroArch::getCorePath() {
  return RetroArch::getBasePath() / _NFS("cores");
}

fs::path RetroArch::getConfigPath() { return BaseDir::data(); }

fs::path RetroArch::getExePath() {
#if defined(_WIN32)
  return BaseDir::data() / L"retroarch" / L"RetroArch.exe";
#elif defined(__APPLE__)
  return BaseDir::data() / "retroarch" / "RetroArch.app";
#elif defined(FLATPAK_VERSION)
  return BaseDir::program() / "retroarch";
#else
  return BaseDir::data() / "appimage" / "RetroArch-Linux-x86_64.AppImage";
#endif
}

fs::path RetroArch::getTemporaryRomPath() {
  return BaseDir::temp() / _NFS("roms");
}

void RetroArch::migrateSaves() {
  std::map<string, string> configs;
  loadConfigHelper(BaseDir::data() / _NFS("retroarch.cfg"), configs);
  fs::error_code err;

  if (configs.count("savefile_directory") > 0) {
    const fs::path oldSaveDir =
        expand_path(fs::to_path(configs["savefile_directory"]));
    if (fs::isDirectorySafe(oldSaveDir)) {
      const fs::path newSaveDir = RetroArch::getBasePath() / _NFS("saves");
      for (const auto &save : fs::directory_iterator(oldSaveDir)) {
        fs::copy(save.path(), newSaveDir / save.path().filename(), err);
      }
    }
  }

  if (configs.count("savestate_directory") > 0) {
    const fs::path oldStateDir =
        expand_path(fs::to_path(configs["savestate_directory"]));
    if (fs::isDirectorySafe(oldStateDir)) {
      const fs::path newStateDir = RetroArch::getBasePath() / _NFS("states");
      for (const auto &state : fs::directory_iterator(oldStateDir)) {
        fs::copy(state.path(), newStateDir / state.path().filename(), err);
      }
    }
  }
}

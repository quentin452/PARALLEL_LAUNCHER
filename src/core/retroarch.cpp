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
  std::map<string, string> configs;
  loadConfig(configs);

  EmulatorCore emulator = romInfo.emulator;
  if (emulator == EmulatorCore::UseDefault) {
    emulator = settings.defaultEmulator;
  }

  GfxPlugin plugin;
  if (emulator == EmulatorCore::ParallelN64) {
    plugin = romInfo.parallelPlugin;
    if (plugin == GfxPlugin::UseDefault) {
      plugin = settings.defaultParallelPlugin;
    }
  } else {
    plugin = romInfo.mupenPlugin;
    if (plugin == GfxPlugin::UseDefault) {
      plugin = settings.defaultMupenPlugin;
    }
  }

  if (pluginOut != nullptr) {
    *pluginOut = plugin;
  }

  WidescreenMode widescreenMode = romInfo.widescreen;
  if (widescreenMode == WidescreenMode::ViewportHack &&
      plugin != GfxPlugin::GLideN64 && plugin != GfxPlugin::OGRE) {
    widescreenMode = WidescreenMode::Off;
  }

  const bool deckMode = useDeckMode(widescreenMode, romInfo);
  const bool deck1610 = use1610(widescreenMode, settings.fullscreen);

  ubyte windowScale = (int)settings.windowScale;
  if (deckMode) {
    windowScale = 4;
  } else if (settings.fullscreen) {
    int screenWidth, screenHeight;
    Screen::getResolution(screenWidth, screenHeight);

    const int maxScale =
        (widescreenMode == WidescreenMode::Off)
            ? std::min(screenWidth / 320, screenHeight / 240)
            : std::min(screenWidth * 3 / 1280, screenHeight / 240);

    windowScale = (ubyte)std::clamp(maxScale, 1, 8);
  }

  string deckModeWidth = "";
  string deckModeHeight = "";
  if (deckMode) {
    int desktopWidth, desktopHeight;
    Screen::getDesktopSize(desktopWidth, desktopHeight);

    deckModeWidth = Number::toString(desktopHeight * 4 / 3);
    deckModeHeight = Number::toString(desktopHeight);
  }

  const string width = Number::toString(320 * (int)windowScale);
  const string height = Number::toString(240 * (int)windowScale);
  const string widescreenWidth =
      deck1610 ? Number::toString(windowScale * 384)
               : Number::toString(
                     (int)std::round((double)windowScale * 1280.0 / 3.0));
  const bool isWidescreen = (widescreenMode != WidescreenMode::Off);
  pl_assert(emulator != EmulatorCore::UseDefault);
  pl_assert(plugin != GfxPlugin::UseDefault);

  const bool emulateFrameBuffer =
      romInfo.glidenFramebufferEmulation && plugin != GfxPlugin::OGRE;
  const bool emulateDepthBuffer =
      romInfo.glidenCorrectDepthCompare && emulateFrameBuffer;

  if (!saveGroup.empty()) {
    configs["savefile_directory"] =
        (RetroArch::getBasePath() / _NFS("saves") / _NFS("sync")).u8string();
  }

  configs["systemfiles_in_content_dir"] = "false";
  configs["video_fullscreen"] =
      (settings.fullscreen && !deckMode) ? "true" : "false";
  configs["video_windowed_fullscreen"] = "true";
  configs["video_window_show_decorations"] = deckMode ? "false" : "true";
  configs["suspend_screensaver_enable"] = "true";
  configs["video_force_aspect"] = isWidescreen ? "false" : "true";
  configs["video_window_save_positions"] = "true";
  configs["video_smooth"] =
      (settings.fullscreen || widescreenMode == WidescreenMode::Stretched)
          ? "true"
          : "false";
  configs["parallel-n64-cpucore"] = configs["mupen64plus-cpucore"] =
      romInfo.useInterpreter ? "pure_interpreter" : "dynamic_recompiler";
  configs["parallel-n64-gfxplugin-accuracy"] = "veryhigh";
  configs["parallel-n64-parallel-rdp-native-texture-lod"] = "enabled";
  configs["mupen64plus-parallel-rdp-native-texture-lod"] = "True";
  configs["global_core_options"] = "true";
  configs["config_save_on_exit"] = "true";
  configs["save_file_compression"] = "false";
  configs["video_disable_composition"] = "false";
  configs["mupen64plus-ForceDisableExtraMem"] = "False";
  configs["savefiles_in_content_dir"] = "false";
  configs["sort_savefiles_by_content_enable"] = "false";
  configs["sort_savefiles_enable"] = "false";
  configs["audio_sync"] = "true";

  configs["parallel-n64-allow-unaligned-dma"] =
      (romInfo.crc32 == 0x0712C306U) ? "False" : "True";
  configs["autosave_interval"] = "4";
  configs["parallel-n64-ISViewer"] =
      settings.enableIsViewer ? "parallel" : "silent";
  configs["parallel-n64-sdcard"] =
      (!romInfo.sdCard.empty() && romInfo.sdCard != "?") ? "SummerCart64"
                                                         : "disabled";

  switch (widescreenMode) {
  case WidescreenMode::Stretched:
    configs["mupen64plus-aspect"] = "16:9";
    break;
  case WidescreenMode::ViewportHack:
    configs["mupen64plus-aspect"] = "16:9 adjusted";
    break;
  default:
    configs["mupen64plus-aspect"] = "4:3";
    break;
  }

  configs["video_window_custom_size_enable"] = "true";
  configs["video_scale_integer"] = "false";
  configs["video_vsync"] = settings.vsync ? "true" : "false";
  configs["video_windowed_position_width"] = configs["custom_viewport_width"] =
      deckMode ? deckModeWidth : (isWidescreen ? widescreenWidth : width);
  configs["video_windowed_position_height"] =
      configs["custom_viewport_height"] = deckMode ? deckModeHeight : height;
  if (deckMode) {
    configs["video_windowed_position_x"] =
        configs["video_windowed_position_y"] = "0";
  }
  configs["aspect_ratio_index"] = isWidescreen ? (deck1610 ? "2" : "1") : "22";
  configs["parallel-n64-aspectratiohint"] =
      isWidescreen ? "widescreen" : "normal";
  configs["parallel-n64-screensize"] = width + "x" + height;
  configs["mupen64plus-43screensize"] = width + "x" + height;
  configs["mupen64plus-169screensize"] =
      getMupenWidescreenResolution(windowScale);
  configs["parallel-n64-gliden64-viewport-hack"] =
      (widescreenMode == WidescreenMode::ViewportHack)
          ? (deck1610 ? "steamdeck" : "enabled")
          : "disabled";
  configs["parallel-n64-framerate"] =
      romInfo.overclockCPU ? "fullspeed" : "original";
  configs["mupen64plus-Framerate"] =
      romInfo.overclockCPU ? "Fullspeed" : "Original";
  configs["mupen64plus-CountPerOp"] = romInfo.overclockCPU ? "1" : "2";
  configs["parallel-n64-virefresh"] = romInfo.overclockVI ? "2200" : "auto";
  configs["mupen64plus-virefresh"] = romInfo.overclockVI ? "2200" : "Auto";
  configs["parallel-n64-parallel-rdp-upscaling"] =
      Number::toString(
          (int)resolveUpscaling(settings.parallelUpscaling, windowScale)) +
      "x";
  configs["mupen64plus-parallel-rdp-upscaling"] =
      Number::toString(
          (int)resolveUpscaling(settings.parallelUpscaling, windowScale)) +
      "x";
  configs["parallel-n64-parallel-rdp-vi-aa"] =
      configs["parallel-n64-parallel-rdp-vi-bilinear"] =
          settings.parallelAntiAliasing ? "enabled" : "disabled";
  configs["mupen64plus-parallel-rdp-vi-aa"] =
      configs["mupen64plus-parallel-rdp-vi-bilinear"] =
          settings.parallelAntiAliasing ? "True" : "False";
  configs["parallel-n64-angrylion-vioverlay"] =
      configs["mupen64plus-angrylion-vioverlay"] =
          getAngrylionFilteringString(settings.angrylionFiltering);
  configs["parallel-n64-parallel-rdp-native-tex-rect"] =
      romInfo.parallelTexRectUpscaling ? "disabled" : "enabled";
  configs["mupen64plus-parallel-rdp-native-tex-rect"] =
      romInfo.parallelTexRectUpscaling ? "False" : "True";
  configs["parallel-n64-remove-vi-borders"] =
      romInfo.parallelRemoveBorders ? "enabled" : "disabled";
  configs["mupen64plus-BilinearMode"] =
      settings.glidenThreePointFiltering ? "3point" : "standard";
  configs["parallel-n64-gliden64-BilinearMode"] =
      settings.glidenThreePointFiltering ? "3point" : "standard";
  configs["mupen64plus-EnableFBEmulation"] =
      configs["parallel-n64-gliden64-EnableFBEmulation"] =
          emulateFrameBuffer ? "True" : "False";
  configs["mupen64plus-EnableN64DepthCompare"] =
      configs["parallel-n64-gliden64-EnableN64DepthCompare"] =
          emulateDepthBuffer ? "Compatible" : "False";
  configs["parallel-n64-gliden64-EnableFragmentDepthWrite"] = "True";
#ifdef __APPLE__
  configs["mupen64plus-FXAA"] = configs["parallel-n64-gliden64-FXAA"] = "0";
  configs["mupen64plus-MultiSampling"] =
      configs["parallel-n64-gliden64-MultiSampling"] = "0";
#else
  configs["mupen64plus-FXAA"] = configs["parallel-n64-gliden64-FXAA"] =
      (settings.glidenAntiAliasing && emulateFrameBuffer) ? "1" : "0";
  configs["mupen64plus-MultiSampling"] =
      configs["parallel-n64-gliden64-MultiSampling"] =
          (settings.glidenAntiAliasing && !emulateFrameBuffer) ? "4" : "0";
#endif
  configs["parallel-n64-gfxplugin"] = s_plugins[(int)plugin];
  if (emulator == EmulatorCore::Mupen64plusNext &&
      plugin == GfxPlugin::GLideN64) {
    configs["mupen64plus-rdp-plugin"] = "gln64";
  } else {
    configs["mupen64plus-rdp-plugin"] = s_plugins[(int)plugin];
  }
  configs["mupen64plus-rsp-plugin"] =
      (plugin == GfxPlugin::GLideN64) ? "hle" : "parallel";
  configs["parallel-n64-rspplugin"] =
      (plugin == GfxPlugin::Angrylion || plugin == GfxPlugin::ParaLLEl ||
       (plugin == GfxPlugin::GLideN64 && romInfo.glidenParallelRsp))
          ? "parallel"
          : "hle";
  configs["parallel-n64-gliden64-LegacySm64ToolsHacks"] =
      (plugin == GfxPlugin::OGRE) ? "enabled" : "disabled";
  configs["parallel-n64-filtering"] = "automatic";
  configs["mupen64plus-EnableLODEmulation"] = "True";
  configs["parallel-n64-gliden64-EnableLODEmulation"] = "True";
  configs["mupen64plus-EnableCopyAuxToRDRAM"] = "True";
  configs["parallel-n64-gliden64-EnableCopyAuxToRDRAM"] = "True";
  configs["parallel-n64-OverrideSaveType"] =
      romInfo.bigEEPROM ? "EEPROM_16KB" : "IGNORE";
  configs["input_autodetect_enable"] = "false";
  configs["game_specific_options"] = "false";
  configs["video_font_enable"] =
      settings.hideRetroArchNotifications ? "false" : "true";
  configs["menu_show_load_content_animation"] = "false";
  configs["pause_nonactive"] = settings.pauseOnBlur ? "true" : "false";
  configs["discord_allow"] = "false";
  configs["parallel-n64-rtc-savestate"] =
      settings.rtcSavestates ? "enabled" : "disabled";
  configs["parallel-n64-gliden64-IniBehaviour"] = "disabled";

#ifdef _WIN32
  switch (settings.audioDriver) {
  case AudioDriver::XAudio2:
    configs["audio_driver"] = "xaudio";
    break;
  case AudioDriver::DirectSound:
    configs["audio_driver"] = "dsound";
    break;
  case AudioDriver::WASAPI:
    configs["audio_driver"] = "wasapi";
    break;
  }
#endif

  configs["parallel-n64-astick-deadzone"] = "0";
  configs["parallel-n64-astick-sensitivity"] = "100";
  configs["mupen64plus-astick-deadzone"] = "0";
  configs["mupen64plus-astick-sensitivity"] = "100";

#ifdef LEGACY_MAPPINGS
  configs["parallel-n64-alt-map"] = "disabled";
  configs["mupen64plus-alt-map"] = "False";
#else
  configs["parallel-n64-alt-map"] = "enabled";
  configs["mupen64plus-alt-map"] = "True";
#endif

  configs["log_verbosity"] = "true";
  configs["log_to_file"] = "true";
  configs["log_to_file_timestamp"] = "false";
  configs["libretro_log_level"] = "3";
  configs["frontend_log_level"] = "3";

  const string numericLocale = std::setlocale(LC_NUMERIC, nullptr);
  std::setlocale(LC_NUMERIC, "C");

  char floatStr[16];
  std::snprintf(floatStr, sizeof(floatStr), "%0.6f",
                players.at(0).profile.deadzone);
  configs["input_analog_deadzone"] = string(floatStr);
  std::snprintf(floatStr, sizeof(floatStr), "%0.6f",
                players.at(0).profile.sensitivity);
  configs["input_analog_sensitivity"] = string(floatStr);

  const float refreshRate = Screen::getRefreshRate();
  if (refreshRate >= 60.f) {
    std::snprintf(floatStr, sizeof(floatStr), "%0.6f", refreshRate);
    configs["video_refresh_rate"] = string(floatStr);
  }

  std::setlocale(LC_NUMERIC, numericLocale.c_str());

  const InputMode inputMode = getInputMode(romInfo.inputModeId);
  const bool multiPort = inputMode.usesTwoPorts();

  const string throawayPath = (BaseDir::data() / _NFS(".temp")).u8string();
  configs["input_remap_binds_enable"] = "false";
  configs["input_remapping_directory"] = throawayPath;
  configs["joypad_autoconfig_dir"] = throawayPath;
  configs["rgui_config_directory"] = throawayPath;
  configs["auto_remaps_enable"] = "false";
  switch (settings.inputDriver) {
  case InputDriver::HID:
    configs["input_joypad_driver"] = "hid";
    break;
  case InputDriver::SDL:
    configs["input_joypad_driver"] = "sdl2";
    break;
  case InputDriver::DirectInput:
    configs["input_joypad_driver"] = "dinput";
    break;
  case InputDriver::JoyDev:
    configs["input_joypad_driver"] = "linuxraw";
    break;
  default:
    configs["input_joypad_driver"] = "null";
    break;
  }
  configs["input_max_users"] =
      Number::toString((int)players.size() * (multiPort ? 2 : 1));

#ifdef _WIN32
  configs["input_driver"] = "dinput";
#endif

  fs::error_code err;
  fs::remove_all(BaseDir::data() / _NFS(".temp"), err);

  // Stub out controller bindings for unused ports
  for (char i = '1'; i <= '4'; i++) {
    configs["parallel-n64-pak"s + i] = "none";
    configs["mupen64plus-pak"s + i] = "none";

    const string prefix = "input_player"s + i + '_';
    const Binding nullBinding{BindingType::None, 0};
    configs[prefix + "joypad_index"] = "nul";
    configs[prefix + "mouse_index"] = "0";

    for (int j = 0; j < (int)ControllerAction::NUM_INPUTS; j++) {
      bindInput(configs, prefix + s_inputNames[j], nullBinding);
    }
    bindInput(configs, prefix + "turbo", nullBinding);
    for (size_t i = 0; i < sizeof(s_unusedInputs) / sizeof(char *); i++) {
      bindInput(configs, prefix + s_unusedInputs[i], nullBinding);
    }
  }

  int fallbackPort;
  HashMap<Uuid, std::queue<int>> devicePorts = getDevicePorts(fallbackPort);

  std::vector<string> deviceTypeArgs;

  bool enableRumble = false;
  bool usesMouse = false;
  for (int i = 0; i < (int)players.size(); i++) {
    const char pc1 = '1' + (char)(multiPort ? 2 * i : i);
    const char pc2 = pc1 + (char)1;

    const string indexKey1 = "input_player"s + pc1 + "_joypad_index";
    const string indexKey2 = "input_player"s + pc2 + "_joypad_index";
    if (!devicePorts[players[i].deviceUuid].empty()) {
      const string deviceIndex =
          Number::toString(devicePorts.at(players[i].deviceUuid).front());
      configs[indexKey1] = deviceIndex;
      if (multiPort) {
        configs[indexKey2] = deviceIndex;
      }
      devicePorts.at(players[i].deviceUuid).pop();
    } else {
      const string deviceIndex = Number::toString(fallbackPort++);
      configs[indexKey1] = deviceIndex;
      if (multiPort) {
        configs[indexKey2] = deviceIndex;
      }
    }

    const string rumbleKey1 = "parallel-n64-pak"s + pc1;
    const string rumbleKey2 = "mupen64plus-pak"s + pc1;
    if (players[i].profile.rumble &&
        romInfo.inputTypes[i] != N64InputType::MemPak) {
      configs[rumbleKey1] = configs[rumbleKey2] = "rumble";
    } else {
      configs[rumbleKey1] = configs[rumbleKey2] = "memory";
    }
    enableRumble |= players[i].profile.rumble;

    const Binding *bindings = players[i].profile.bindings;
    const Binding nullBinding = {BindingType::None, 0};
    for (int j = 0; j < (int)ControllerAction::NUM_INPUTS; j++) {
      const string key = "input_player"s + pc1 + '_' + s_inputNames[j];
      const ControllerAction vi[2] = {inputMode.port1[j], inputMode.port2[j]};

      bindInput(configs, key,
                (vi[0] == ControllerAction::None) ? nullBinding
                                                  : bindings[(ubyte)vi[0]]);
      if (multiPort) {
        const string key = "input_player"s + pc2 + '_' + s_inputNames[j];
        bindInput(configs, key,
                  (vi[1] == ControllerAction::None) ? nullBinding
                                                    : bindings[(ubyte)vi[1]]);
      }
    }

    bindInput(configs, "input_player"s + pc1 + "_turbo"s,
              {BindingType::None, 0});
    for (size_t i = 0; i < sizeof(s_unusedInputs) / sizeof(char *); i++) {
      bindInput(configs, "input_player"s + pc1 + s_unusedInputs[i],
                nullBinding);
    }

    if (multiPort) {
      bindInput(configs, "input_player"s + pc2 + "_turbo"s,
                {BindingType::None, 0});
      for (size_t i = 0; i < sizeof(s_unusedInputs) / sizeof(char *); i++) {
        bindInput(configs, "input_player"s + pc2 + s_unusedInputs[i],
                  nullBinding);
      }
    }

    switch (romInfo.inputTypes[i]) {
    case N64InputType::Gamecube:
    case N64InputType::GamecubeKeyboard:
      configs["input_libretro_device_p"s + pc1] = "5";
      deviceTypeArgs.push_back("-d "s + pc1 + ":5");
      break;
    case N64InputType::Mouse:
      configs["input_libretro_device_p"s + pc1] = "2";
      deviceTypeArgs.push_back("-d "s + pc1 + ":2");
      usesMouse = true;
      break;
    default:
      configs["input_libretro_device_p"s + pc1] = "1";
      break;
    }
  }

  if (bindSavestate) {
    bindInput(configs, "input_load_state",
              players.at(0).profile.bindings[(int)ControllerAction::LoadState]);
    bindInput(configs, "input_save_state",
              players.at(0).profile.bindings[(int)ControllerAction::SaveState]);
    bindInput(
        configs, "input_toggle_fast_forward",
        players.at(0).profile.bindings[(int)ControllerAction::FastForward]);
    bindInput(
        configs, "input_toggle_slowmotion",
        players.at(0).profile.bindings[(int)ControllerAction::SlowMotion]);
  } else {
    configs["input_load_state_btn"] = "nul";
    configs["input_load_state_axis"] = "nul";
    configs["input_save_state_btn"] = "nul";
    configs["input_save_state_axis"] = "nul";
  }

  if (players.size() >= 1) {
    configs["input_axis_threshold"] =
        Number::toString(players.at(0).profile.axisThreshold);
  } else {
    configs["input_axis_threshold"] = "0.50";
  }

  configs["enable_device_vibration"] = enableRumble ? "true" : "false";
  configs["input_auto_mouse_grab"] = usesMouse ? "true" : "false";

  configs["input_desktop_menu_toggle"] = "nul";
  configs["input_game_focus_toggle"] = "nul";
  configs["input_send_debug_info"] = "nul";
  configs["input_shader_next"] = "nul";
  configs["input_shader_prev"] = "nul";
  configs["input_netplay_game_watch"] = "nul";
  configs["input_netplay_host_toggle"] = "nul";
  configs["input_osk_toggle"] = "nul";

  for (size_t i = 0; i < sizeof(s_unusedInputs) / sizeof(char *); i++) {
    configs["input_player1_"s + s_unusedInputs[i]] = "nul";
  }

  const std::vector<KeyId> hotkeys = FileController::loadHotkeys();
  pl_assert(hotkeys.size() == (size_t)Hotkey::NUM_HOTKEYS);
  for (ubyte i = 0; i < (ubyte)Hotkey::NUM_HOTKEYS; i++) {
    configs[Hotkeys::ConfigNames[i]] =
        Keycode::getNames(hotkeys[i]).retroConfigName;
  }

  saveConfig(configs);

  const char *const coreName = (emulator == EmulatorCore::ParallelN64)
                                   ? "parallel_n64_next_libretro"
                                   : "mupen64plus_next_libretro";
  fs::path corePath = RetroArch::getCorePath() / coreName;

#if defined(_WIN32)
  corePath += L".dll";
#elif defined(__APPLE__)
  corePath += ".dylib";
#else
  corePath += ".so";
#endif

  if (!saveGroup.empty()) {
    fs::path tempRomPath = RetroArch::getTemporaryRomPath();
    fs::error_code err;
    fs::create_directories(tempRomPath, err);
    tempRomPath = tempRomPath / saveGroup;
    tempRomPath.replace_extension(romPath.extension());
    fs::remove(tempRomPath, err);
    err.clear();
    fs::copy_file(romPath, tempRomPath, err);
    if (err) {
      logError("Failed to copy rom file from "s + romPath.u8string() + " to " +
               tempRomPath.u8string() + ": \n" + err.message());
    }
    romPath = tempRomPath;
  }

  if (!fs::existsSafe(romPath)) {
    logError("ROM not found: "s + romPath.u8string());
  }

  if (settings.enableIsViewer) {
    Environment::set("PL_ISV_FILE_PATH", isViewerFile.c_str());
  } else {
    Environment::clear("PL_ISV_FILE_PATH");
  }

  if (!romInfo.sdCard.empty() && romInfo.sdCard != "?") {
    Environment::set("PL_SD_CARD_IMAGE", (BaseDir::data() / _NFS("sdcard") /
                                          fs::to_path(romInfo.sdCard + ".iso"))
                                             .u8string()
                                             .c_str());
  } else {
    Environment::clear("PL_SD_CARD_IMAGE");
  }

  if (inputBindingError) {
    Environment::set(
        "PL_ALERT_MESSAGE",
        QCoreApplication::translate(
            "RetroArch",
            "[Parallel Launcher] Input driver encountered an unexpected error. "
            "Connected controllers may not be mapped correctly.")
            .toStdString()
            .c_str());
  } else {
    Environment::clear("PL_ALERT_MESSAGE");
  }

  Environment::set("PL_RTC_OFFSET",
                   Number::toString(settings.rtcOffset).c_str());
  Environment::set("PL_LIBPL_PIPE_OUT", libplInputPipe.c_str());
  Environment::set("PL_LIBPL_PIPE_IN", libplOutputPipe.c_str());

#ifdef _WIN32
  Environment::set("SDL_JOYSTICK_RAWINPUT", settings.sdlRawInput ? "1" : "0");
#endif

#if defined(__linux__) && !defined(FLATPAK_VERSION)

  const bool usingController =
      ((players.size() > 1) ||
       (players.size() == 1 && players.at(0).deviceUuid != Uuid::empty()));

  if (usingController && settings.inputDriver == InputDriver::SDL &&
      SdlDriver::getSdlVersion() > Version{2, 0, 4}) {
    /* This doesn't work because libSDL2 has dependencies */
    // Environment::set( "LD_PRELOAD", SdlDriver::getSdlLibraryPath().c_str() );

    const string sdlLibraryPath = SdlDriver::getSdlLibraryPath();
    if (!sdlLibraryPath.empty()) {
      Environment::set(
          "LD_LIBRARY_PATH",
          fs::path(sdlLibraryPath).parent_path().u8string().c_str());
    }
  }
#endif

  std::vector<string> args;
  args.reserve(8);
#ifdef __APPLE__
  string launchBinary =
      (RetroArch::getExePath() / "Contents" / "MacOS" / "RetroArch").u8string();
  if (AppleUtil::shouldUseArmCore()) {
    args.insert(args.end(), "-arm64");
    args.insert(args.end(), launchBinary);
    launchBinary = "arch";
  }
#endif
  args.insert(args.end(),
              {"-L"s, corePath.u8string(), "--config"s,
               (BaseDir::data() / _NFS("retroarch.cfg")).u8string()});
  args.insert(args.end(), deviceTypeArgs.begin(), deviceTypeArgs.end());
  args.push_back(romPath.u8string());

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

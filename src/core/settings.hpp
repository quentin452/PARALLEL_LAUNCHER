#ifndef SRC_CORE_SETTINGS_HPP_
#define SRC_CORE_SETTINGS_HPP_

#include "src/core/filesystem.hpp"
#include "src/core/flags.hpp"
#include "src/core/json.hpp"
#include "src/core/uuid.hpp"
#include "src/input/input-driver.hpp"
#include <optional>

enum class GfxPlugin : ubyte {
  UseDefault = 0,
  ParaLLEl = 1,
  Glide64 = 2,
  Angrylion = 3,
  GLideN64 = 4,
  Rice = 5,
  Gln64 = 6,
  OGRE = 7
};

enum class ParallelUpscaling : ubyte {
  Auto = 0,
  None = 1,
  x2 = 2,
  x4 = 4,
  x8 = 8
};

enum class AngrylionFiltering : ubyte {
  None = 0,
  AntiAlias = 1,
  Dedither = 2,
  Blur = 3,
  Filter = 4
};

enum class RomInfoColumn : ubyte {
  Path = 0x1,
  LastPlayed = 0x2,
  PlayTime = 0x4,
  // Notes = 0x8, -- Removed
  InternalName = 0x10
};

enum class EmulatorCore : ubyte {
  UseDefault = 0,
  ParallelN64 = 1,
  Mupen64plusNext = 2
};

DEFINE_FLAG_OPERATIONS(RomInfoColumn, ubyte)

enum class CoreUpdateInterval : ubyte {
  EveryLaunch = 0,
  Daily = 1,
  Weekly = 2,
  Monthly = 3
};

#ifdef _WIN32
enum class AudioDriver : ubyte { XAudio2, DirectSound, WASAPI };
#endif

struct AppSettings {
  RomInfoColumn visibleColumns;
  GfxPlugin defaultParallelPlugin;
  GfxPlugin defaultMupenPlugin;
  EmulatorCore defaultEmulator;
  ParallelUpscaling parallelUpscaling;
  bool parallelAntiAliasing;
  bool parallelTexRectUpscaling;
  bool parallelRemoveBorders;
  AngrylionFiltering angrylionFiltering;
  bool glidenAntiAliasing;
  bool glidenThreePointFiltering;
  bool glidenCorrectDepthCompare;
  bool glidenFramebufferEmulation;
  ubyte windowScale;
  bool vsync;
  bool hideWhenPlaying;
  std::optional<Uuid> preferredController;
#ifndef __linux__
  string theme;
  bool checkForUpdates;
  bool darkMode;
#endif
  bool patchToSameFolder;
  fs::path patchedRomFolder;
  short migrationState;
  CoreUpdateInterval coreUpdateInterval;
  bool mupenDevBranch;
  bool pauseOnBlur;
  string locale;
  bool discordIntegration;
  InputDriver inputDriver;
#ifdef _WIN32
  bool sdlRawInput;
#endif
  bool hideRetroArchNotifications;
  bool showAdvancedSettings;
  bool knowsAboutRhdcView;
  bool ignoreGamecubeWarnings;
  bool fullscreen;
#ifdef _WIN32
  AudioDriver audioDriver;
#endif
#ifdef __APPLE__
  bool forceX86;
#endif
  bool enableIsViewer;
  int isViewerHistorySize;
  bool emulateSdCard;
  int rtcOffset;
  bool rtcSavestates;

  static const AppSettings Default;
};

namespace JsonSerializer {
template <> void serialize<AppSettings>(JsonWriter &jw, const AppSettings &obj);
template <> AppSettings parse<AppSettings>(const Json &json);
} // namespace JsonSerializer

#endif /* SRC_CORE_SETTINGS_HPP_ */

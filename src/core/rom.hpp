#ifndef SRC_CORE_ROM_HPP_
#define SRC_CORE_ROM_HPP_

#include "src/core/filesystem.hpp"
#include "src/core/json.hpp"
#include "src/core/settings.hpp"
#include "src/core/uuid.hpp"
#include "src/rhdc/core/hack.hpp"
#include <array>
#include <optional>
#include <set>

enum class WidescreenMode : ubyte { Off = 0, Stretched = 1, ViewportHack = 2 };

enum class N64InputType : ubyte {
  Unknown = 0x00,
  RumblePak = 0x01,
  MemPak = 0x02,
  TransferPak = 0x03,
  Mouse = 0x80,
  VRU = 0x81,
  Gamecube = 0x82,
  RandnetKeyboard = 0x83,
  GamecubeKeyboard = 0x84,
  None = 0xff
};

typedef std::array<N64InputType, 4> N64InputTypes;
static_assert(sizeof(N64InputTypes) == 4);

extern const N64InputTypes UNCHECKED_INPUT_TYPES;

struct RomFile {
  fs::path path;
  int64 lastModified;
  string sha1;
  bool local;
};

struct RomInfo {
  string sha1;
  string name;
  string internalName;
  EmulatorCore emulator;
  GfxPlugin parallelPlugin;
  GfxPlugin mupenPlugin;
  bool parallelTexRectUpscaling;
  bool parallelRemoveBorders;
  bool glidenFramebufferEmulation;
  bool glidenCorrectDepthCompare;
  bool glidenParallelRsp;
  int64 lastPlayed;
  int64 playTime;
  std::set<string> groups;
  bool overclockCPU;
  bool overclockVI;
  uint crc32;
  Uuid inputModeId;
  WidescreenMode widescreen;
  bool bigEEPROM;
  bool useInterpreter;
  N64InputTypes inputTypes;
  string sdCard;
};

struct ConstRomReference {
  const RomFile *file;
  const RomInfo *info;
  const BasicRhdcHack *rhdc;
};

struct RomReference {
  RomFile *file;
  RomInfo *info;
  BasicRhdcHack *rhdc;

  inline ConstRomReference toConst() const noexcept {
    return ConstRomReference{file, info, rhdc};
  }
};

struct RomSource {
  fs::path folder;
  bool recursive;
  bool ignoreHidden;
  bool followSymlinks;
  ubyte maxDepth;
  std::set<string> autoTags;
};

namespace JsonSerializer {
template <> void serialize<RomSource>(JsonWriter &jw, const RomSource &obj);
template <> RomSource parse<RomSource>(const Json &json);

template <> void serialize<fs::path>(JsonWriter &jw, const fs::path &obj);
template <> fs::path parse<fs::path>(const Json &json);
} // namespace JsonSerializer

namespace RomUtil {
extern string getInternalName(const fs::path romPath);
extern int64 getLastModified(const fs::path romPath);
extern uint getCrc32(const fs::path &romPath);

extern bool coveredBySearchPath(const fs::path &romPath);
extern N64InputTypes readControllerTypes(const fs::path &romPath);
} // namespace RomUtil

#endif /* SRC_CORE_ROM_HPP_ */

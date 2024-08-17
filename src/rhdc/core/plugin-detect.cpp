#include "src/rhdc/core/plugin-detect.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/version.hpp"
#include "src/polyfill/byteswap.hpp"
#include "src/polyfill/file.hpp"

enum class Tool {
  DecompProbably,
  BowsersBlueprints,
  BowsersBlueprintsOld,
  RomManager,
  SM64Editor,
  SM64EditorPlusMOP
};

static constexpr uint BBP_SIGNATURE = 0x4242503A;
static constexpr uint SME_SIGNATURE = 0x170C0007;
static constexpr uint RM_SIGNATURE = 0x00AED760;

static inline Tool guessTool(InputFile &rom) {
  uint bbpSignature = 0;
  rom.seekg(-8, std::ios_base::end);
  rom.read((char *)&bbpSignature, 4);

  /* Bowser's Blueprints */
  if (BBP_SIGNATURE == htonl(bbpSignature)) {
    uint metadataAddr = 0;
    rom.read((char *)&metadataAddr, 4);
    rom.seekg((size_t)ntohl(metadataAddr) + 6);

    ushort versionData[3] = {0, 0, 0};
    rom.read((char *)versionData, 6);

    const Version bbpRomVersion = {ntohs(versionData[0]), ntohs(versionData[1]),
                                   ntohs(versionData[2])};

    if (bbpRomVersion >= Version{0, 4, 1}) {
      return Tool::BowsersBlueprints;
    } else {
      return Tool::BowsersBlueprintsOld;
    }
  }

  /* Rom Manager */
  rom.seekg(0x269EC4);
  uint rmSignature = 0;
  rom.read((char *)&rmSignature, 4);
  if (RM_SIGNATURE == ntohl(rmSignature)) {
    return Tool::RomManager;
  }

  /* SM64 Editor */
  rom.seekg(0x405CC8);
  uint smeSignature = 0;
  rom.read((char *)&smeSignature, 4);
  if (SME_SIGNATURE == ntohl(smeSignature)) {
    return Tool::SM64Editor;
  }

  return Tool::DecompProbably;
}

static AutoPlugin makeAutoPlugin(EmulatorCore emulator,
                                 GfxPlugin parallelPlugin,
                                 GfxPlugin mupenPlugin) {
  const AppSettings &settings = FileController::loadAppSettings();
  return AutoPlugin{emulator,
                    parallelPlugin,
                    mupenPlugin,
                    settings.parallelTexRectUpscaling,
                    settings.glidenFramebufferEmulation,
                    settings.glidenCorrectDepthCompare,
                    false};
}

static inline AutoPlugin guess(const fs::path &romPath, bool preferHle) {
  InputFile romFile(romPath, true);
  switch (guessTool(romFile)) {
  case Tool::BowsersBlueprintsOld:
  case Tool::SM64Editor:
    return makeAutoPlugin(EmulatorCore::ParallelN64, GfxPlugin::OGRE,
                          GfxPlugin::GLideN64);
  case Tool::SM64EditorPlusMOP:
    return makeAutoPlugin(EmulatorCore::ParallelN64, GfxPlugin::Rice,
                          GfxPlugin::GLideN64);
  case Tool::RomManager:
    return makeAutoPlugin(EmulatorCore::ParallelN64, GfxPlugin::GLideN64,
                          GfxPlugin::GLideN64);
  case Tool::DecompProbably:
  case Tool::BowsersBlueprints:
  default:
    return preferHle ? makeAutoPlugin(EmulatorCore::ParallelN64,
                                      GfxPlugin::GLideN64, GfxPlugin::GLideN64)
                     : makeAutoPlugin(EmulatorCore::ParallelN64,
                                      GfxPlugin::ParaLLEl, GfxPlugin::ParaLLEl);
  }
}

AutoPlugin AutoPlugin::detect(const fs::path &romPath,
                              const std::optional<GfxPlugin> recommendedPlugin,
                              PluginFlags pluginFlags) {
  const bool preferHle = FileController::loadRhdcSettings().preferHle;
  if (!recommendedPlugin.has_value()) {
    return guess(romPath, preferHle);
  }

  const AppSettings &settings = FileController::loadAppSettings();
  const GfxPlugin plugin = recommendedPlugin.value();
  AutoPlugin autoPlugin = {settings.defaultEmulator,
                           plugin,
                           plugin,
                           settings.parallelTexRectUpscaling,
                           settings.glidenFramebufferEmulation,
                           settings.glidenCorrectDepthCompare,
                           false};

  switch (plugin) {
  case GfxPlugin::ParaLLEl:
    autoPlugin.upscaleTexrects =
        Flags::has(pluginFlags, PluginFlags::UpscaleTexrects);
    if (Flags::has(pluginFlags, PluginFlags::AllowHleFallback)) {
      autoPlugin.emulateFramebuffer =
          Flags::has(pluginFlags, PluginFlags::EmulateFramebuffer);
      if (autoPlugin.emulateFramebuffer) {
        autoPlugin.accurateDepthCompare =
            Flags::has(pluginFlags, PluginFlags::AcccurateDepthCompare);
      }
      if (preferHle) {
        autoPlugin.parallelPlugin = GfxPlugin::GLideN64;
        autoPlugin.mupenPlugin = GfxPlugin::GLideN64;
      }
      autoPlugin.emulateRsp = Flags::has(pluginFlags, PluginFlags::EmulateRsp);
    } else {
      autoPlugin.emulateFramebuffer = true;
      autoPlugin.accurateDepthCompare = true;
      autoPlugin.emulateRsp = true;
    }
    break;
  case GfxPlugin::GLideN64:
    autoPlugin.emulateFramebuffer =
        Flags::has(pluginFlags, PluginFlags::EmulateFramebuffer);
    if (autoPlugin.emulateFramebuffer) {
      autoPlugin.accurateDepthCompare =
          Flags::has(pluginFlags, PluginFlags::AcccurateDepthCompare);
    }
    autoPlugin.emulateRsp = Flags::has(pluginFlags, PluginFlags::EmulateRsp);
    break;
  case GfxPlugin::Glide64:
  case GfxPlugin::Rice:
  case GfxPlugin::OGRE:
    autoPlugin.emulator = EmulatorCore::ParallelN64;
    autoPlugin.mupenPlugin = GfxPlugin::GLideN64;
    autoPlugin.emulateFramebuffer = false;
    autoPlugin.accurateDepthCompare = false;
    break;
  case GfxPlugin::Angrylion:
    autoPlugin.upscaleTexrects = false;
    autoPlugin.emulateFramebuffer = true;
    autoPlugin.accurateDepthCompare = true;
    break;
  default:
    break;
  }

  return autoPlugin;
}

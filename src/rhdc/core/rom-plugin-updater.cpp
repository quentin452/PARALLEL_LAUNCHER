#include "src/rhdc/core/rom-plugin-updater.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/db/data-provider.hpp"

void RomPluginUpdater::applyRhdcRecommendations(
    const RhdcSettingHintsExt &options) {
  DataProvider::updateRomEmulator(options.sha1, EmulatorCore::ParallelN64);

  fs::path romPath;
  if (options.plugin.has_value()) {
    GfxPlugin plugin = options.plugin.value();

    if (plugin == GfxPlugin::ParaLLEl &&
        Flags::has(options.pluginFlags, PluginFlags::AllowHleFallback) &&
        FileController::loadRhdcSettings().preferHle) {
      plugin = GfxPlugin::GLideN64;
    }

    DataProvider::updateRomParallelPlugin(options.sha1, plugin);
    DataProvider::setRomParallelTexRectUpscaling(
        options.sha1,
        Flags::has(options.pluginFlags, PluginFlags::UpscaleTexrects));
    DataProvider::setRomGlidenFrameBufferEmulation(
        options.sha1,
        Flags::has(options.pluginFlags, PluginFlags::EmulateFramebuffer));
    DataProvider::setRomGlidenAccurateDepthCompare(
        options.sha1,
        Flags::has(options.pluginFlags, PluginFlags::AcccurateDepthCompare));
    DataProvider::setRomGlidenRspEmulation(
        options.sha1, Flags::has(options.pluginFlags, PluginFlags::EmulateRsp));
    DataProvider::updateRomWidescreen(
        options.sha1, Flags::has(options.pluginFlags, PluginFlags::Widescreen)
                          ? WidescreenMode::ViewportHack
                          : WidescreenMode::Off);
  } else if (DataProvider::tryFetchRomPathWithSha1(options.sha1, romPath)) {
    const AutoPlugin autoPlugin =
        AutoPlugin::detect(romPath, options.plugin, options.pluginFlags);

    DataProvider::updateRomParallelPlugin(options.sha1,
                                          autoPlugin.parallelPlugin);
    DataProvider::setRomParallelTexRectUpscaling(options.sha1,
                                                 autoPlugin.upscaleTexrects);
    DataProvider::setRomGlidenFrameBufferEmulation(
        options.sha1, autoPlugin.emulateFramebuffer);
    DataProvider::setRomGlidenAccurateDepthCompare(
        options.sha1, autoPlugin.accurateDepthCompare);
    DataProvider::setRomGlidenFrameBufferEmulation(options.sha1,
                                                   autoPlugin.emulateRsp);
  }

  DataProvider::updateRomCpuOverclocking(
      options.sha1, !Flags::has(options.hackFlags, RhdcHackFlag::NoOverclock));
  DataProvider::updateRomInputMode(
      options.sha1, Flags::has(options.hackFlags, RhdcHackFlag::DualAnalog)
                        ? DefaultInputModes::DualAnalog.id
                        : DefaultInputModes::Normal.id);
  DataProvider::updateRomBigEeprom(
      options.sha1, Flags::has(options.hackFlags, RhdcHackFlag::BigEEPROM));
}

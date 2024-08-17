#ifndef SRC_RHDC_CORE_ROM_PLUGIN_UPDATER_HPP_
#define SRC_RHDC_CORE_ROM_PLUGIN_UPDATER_HPP_

#include "src/rhdc/core/hack.hpp"
#include "src/rhdc/core/plugin-detect.hpp"
#include <optional>

struct RhdcSettingHintsExt {
  string sha1;
  std::optional<GfxPlugin> plugin;
  PluginFlags pluginFlags;
  RhdcHackFlag hackFlags;
};

namespace RomPluginUpdater {

extern void applyRhdcRecommendations(const RhdcSettingHintsExt &options);

}

#endif /* SRC_RHDC_CORE_ROM_PLUGIN_UPDATER_HPP_ */

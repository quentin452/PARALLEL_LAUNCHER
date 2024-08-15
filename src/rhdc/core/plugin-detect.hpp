#ifndef SRC_RHDC_CORE_PLUGIN_DETECT_HPP_
#define SRC_RHDC_CORE_PLUGIN_DETECT_HPP_

#include <optional>
#include "src/core/settings.hpp"
#include "src/rhdc/core/hack.hpp"

enum class PluginFlags : uint_fast8_t {
	None = 0,
	UpscaleTexrects = 0x1,
	AllowHleFallback = 0x2,
	EmulateFramebuffer = 0x4,
	AcccurateDepthCompare = 0x8,
	Widescreen = 0x10,
	EmulateRsp = 0x20
};
DEFINE_FLAG_OPERATIONS( PluginFlags, uint_fast8_t );

struct RhdcHackVersionExt {
	string sha1;
	string name;
	std::optional<GfxPlugin> plugin;
	PluginFlags pluginFlags;
	RhdcHackFlag hackFlags;
	bool archived;
};

struct AutoPlugin {
	EmulatorCore emulator;
	GfxPlugin parallelPlugin;
	GfxPlugin mupenPlugin;
	bool upscaleTexrects;
	bool emulateFramebuffer;
	bool accurateDepthCompare;
	bool emulateRsp;

	static AutoPlugin detect(
		const fs::path &romPath,
		const std::optional<GfxPlugin> recommendedPlugin,
		PluginFlags pluginFlags
	);
};

#endif /* SRC_RHDC_CORE_PLUGIN_DETECT_HPP_ */

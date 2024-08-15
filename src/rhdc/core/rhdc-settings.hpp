#ifndef SRC_RHDC_CORE_SETTINGS_HPP_
#define SRC_RHDC_CORE_SETTINGS_HPP_

#include "src/core/filesystem.hpp"
#include "src/core/json.hpp"

struct RhdcSettings {
	fs::path downloadDirectory;
	bool enableStarDisplay;
	bool checkAllSaveSlots;
	bool preferHle;
	bool enableDefaultLayouts;
	bool ignoreWidescreenHint;

	static const RhdcSettings &Default();
};

namespace JsonSerializer {
	template<> void serialize<RhdcSettings>( JsonWriter &jw, const RhdcSettings &obj );
	template<> RhdcSettings parse<RhdcSettings>( const Json &json );
}

#endif /* SRC_RHDC_CORE_SETTINGS_HPP_ */

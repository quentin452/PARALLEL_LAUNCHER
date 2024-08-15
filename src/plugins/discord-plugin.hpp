#ifndef SRC_PLUGINS_DISCORD_PLUGIN_HPP_
#define SRC_PLUGINS_DISCORD_PLUGIN_HPP_

#include "src/thirdparty/discord_rpc.h"

struct DiscordPlugin {
	void (*initialize)( const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId );
	void (*runCallbacks)(void);
	void (*updatePresence)(const DiscordRichPresence* presence);
	void (*clearPresence)(void);

	static const DiscordPlugin &tryLoad( bool *success ) noexcept;
};

#endif /* SRC_PLUGINS_DISCORD_PLUGIN_HPP_ */

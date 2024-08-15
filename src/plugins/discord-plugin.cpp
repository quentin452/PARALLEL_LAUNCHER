#include "src/plugins/discord-plugin.hpp"

#ifdef DISCORD_LAZY_LINKING

#include <dlfcn.h>
#include <iostream>
#include "src/polyfill/base-directory.hpp"

static void initStub( const char*, DiscordEventHandlers*, int, const char* ) {}
static void updateStub( const DiscordRichPresence* ) {}
static void nop(void) {}

static const DiscordPlugin s_stub = {
	initStub,
	nop,
	updateStub,
	nop
};

static bool s_fullyLoaded = false;
static DiscordPlugin s_abi;

static bool tryLoadFunction( void *handle, void *&funcPtr, const char *funcName ) {
	funcPtr = dlsym( handle, funcName );
	if( funcPtr == nullptr ) {
		std::cerr << "Failed to load symbol '" << funcName << "' from library." << std::endl << std::flush;
		return false;
	}

	return true;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
const DiscordPlugin &DiscordPlugin::tryLoad( bool *success ) noexcept {
	if( s_fullyLoaded ) {
		if( success != nullptr ) *success = true;
		return s_abi;
	}

	void *handle = dlopen( (BaseDir::data() / "plugins" / "libdiscord-rpc.so").u8string().c_str(), RTLD_NOW );
	if( !handle ) {
		if( success != nullptr ) *success = false;
		return s_stub;
	}

	if(
		tryLoadFunction( handle, reinterpret_cast<void*&>( s_abi.initialize ), "Discord_Initialize" ) &&
		tryLoadFunction( handle, reinterpret_cast<void*&>( s_abi.runCallbacks ), "Discord_RunCallbacks" ) &&
		tryLoadFunction( handle, reinterpret_cast<void*&>( s_abi.updatePresence ), "Discord_UpdatePresence" ) &&
		tryLoadFunction( handle, reinterpret_cast<void*&>( s_abi.clearPresence ), "Discord_ClearPresence" )
	) {
		if( success != nullptr ) *success = true;
		return s_abi;
	}

	if( success != nullptr ) *success = false;
	return s_stub;
}
#pragma GCC diagnostic pop

#else

static const DiscordPlugin s_abi = {
	Discord_Initialize,
	Discord_RunCallbacks,
	Discord_UpdatePresence,
	Discord_ClearPresence
};

const DiscordPlugin &DiscordPlugin::tryLoad( bool *success ) noexcept {
	if( success != nullptr ) *success = true;
	return s_abi;
}

#endif

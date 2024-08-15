#include "src/core/steam-deck.hpp"

#ifdef FLATPAK_VERSION

#include <stdlib.h>
#include "src/polyfill/file.hpp"
#include "src/types.hpp"

static inline bool getIsSteamDeck() {
	InputFile osInfo( "/run/host/etc/os-release", false );

	string line;
	while( std::getline( osInfo, line ).good() && !osInfo.eof() ) {
		if( line.length() < 11 ) continue;
		if( line.substr( 0, 11 ) != "VARIANT_ID=" ) continue;

		return line.substr( 11 ) == "steamdeck";
	}

	return false;
}

bool SteamDeck::isSteamDeck() {
	static const bool isDeck = getIsSteamDeck();
	return isDeck;
}

bool SteamDeck::inGamingMode() {
	return isSteamDeck() && getenv( "SteamEnv" ) != nullptr;
}

#else

bool SteamDeck::isSteamDeck() {
	return false;
}

bool SteamDeck::inGamingMode() {
	return false;
}

#endif

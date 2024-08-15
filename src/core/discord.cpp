#include "src/core/discord.hpp"

#include <cstring>
#include <cstdio>
#include <QTimer>
#include "src/plugins/discord-plugin.hpp"
#include "src/core/logging.hpp"
#include "src/core/qthread.hpp"
#include "src/core/time.hpp"

static const char *const APPLICATION_ID = "874416020570374205";

static string s_nowPlaying = ""s;
static int64 s_startTime = 0;
static bool s_connected = false;
static bool s_initialized = false;

static void connect();
static void updateNowPlaying();

static void onInitialized( const DiscordUser* ) {
	QtThread::safeAsync( []() {
		s_connected = true;
		updateNowPlaying();
	});
}

static void onError( int, const char *message ) {
	logError( "[Discord] "s + message );
}

static void onDisconnect( int errorCode, const char *message ) {
	onError( errorCode, message );
	QtThread::safeAsync( []() {
		if( s_nowPlaying.empty() ) {
			s_connected = false;
		} else {
			connect();
		}
	});
}

static void connect() {
	bool pluginLoaded = false;
	const DiscordPlugin &discord = DiscordPlugin::tryLoad( &pluginLoaded );

	if( !pluginLoaded ) return;

	static DiscordEventHandlers eventHandlers = {
		/* ready */ onInitialized,
		/* disconnected */ onDisconnect,
		/* errored */ onError,
		/* joinGame */ nullptr,
		/* spectateGame */ nullptr,
		/* joinRequest */ nullptr
	};

	discord.initialize( APPLICATION_ID, &eventHandlers, 0, nullptr );

	if( !s_initialized ) {
		s_initialized = true;
		QTimer *eventLoop = new QTimer();
		QObject::connect( eventLoop, &QTimer::timeout, discord.runCallbacks );
		eventLoop->setInterval( 1000 );
		eventLoop->setSingleShot( false );
		eventLoop->start();
	}
}

void Discord::setNowPlaying( const string &gameName ) {
	Discord::setNowPlaying( gameName, Time::now() );
}

void Discord::setNowPlaying( const string &gameName, int64 startTime ) {
	QtThread::safeAsync( [gameName, startTime]() {
		s_nowPlaying = gameName;
		s_startTime = startTime;
		if( !s_connected ) {
			connect();
		} else {
			updateNowPlaying();
		}
	});
}

static char s_details[128];

static inline DiscordRichPresence createActivity() {
	DiscordRichPresence activity;
	s_details[0] = '\0';
	std::memset( &activity, 0, sizeof( DiscordRichPresence ) );
	activity.details = s_details;
	activity.largeImageKey = "app-icon";
	activity.largeImageText = "Parallel Launcher";
	return activity;
}

static void updateNowPlaying() {
	QtThread::safeAsync( []() {
		const DiscordPlugin &discord = DiscordPlugin::tryLoad( nullptr );

		if( s_nowPlaying.empty() ) {
			discord.clearPresence();
			return;
		}

		static DiscordRichPresence activity = createActivity();
		std::snprintf( s_details, 128, "Playing \"%s\"", s_nowPlaying.c_str() );
		activity.startTimestamp = s_startTime;
		activity.endTimestamp = 0;

		discord.updatePresence( &activity );
	});
}

bool Discord::pluginInstalled() {
	bool pluginLoaded = false;
	DiscordPlugin::tryLoad( &pluginLoaded );
	return pluginLoaded;
}

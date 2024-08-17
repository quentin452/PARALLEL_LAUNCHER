#include "src/core/settings.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/flags.hpp"
#include "src/core/steam-deck.hpp"
#ifndef __linux__
#include "src/ui/palette.hpp"
#endif

const AppSettings AppSettings::Default = {
    /* visibleColumns */ RomInfoColumn::LastPlayed | RomInfoColumn::PlayTime,
    /* defaultParallelPlugin */ GfxPlugin::ParaLLEl,
    /* defaultMupenPlugin */ GfxPlugin::ParaLLEl,
    /* defaultEmulator */ EmulatorCore::ParallelN64,
    /* parallelUpscaling */ ParallelUpscaling::Auto,
    /* parallelAntiAliasing */ true,
    /* parallelTexRectUpscaling */ false,
    /* parallelRemoveBorders */ false,
    /* angrylionFiltering */ AngrylionFiltering::None,
    /* glidenAntiAliasing */ false,
    /* glidenThreePointFiltering */ true,
    /* glidenCorrectDepthCompare */ true,
    /* glidenFramebufferEmulation */ true,
    /* windowScale */ 4,
#ifdef _WIN32
    /* vsync */ false,
#else
    /* vsync */ true,
#endif
    /* hideWhenPlaying */ false,
    /* preferredController */ std::nullopt,
#ifndef __linux__
    /* theme */ "Fusion",
    /* checkForUpdates */ true,
    /* darkMode */ ColourPalette::systemIsDarkMode(),
#endif
    /* patchToSameFolder */ true,
    /* patchedRomFolder */ fs::to_path("~/Documents/N64 Roms"),
    /* migrationState */ 39,
    /* coreUpdateInterval */ CoreUpdateInterval::Daily,
    /* mupenDevBranch */ true,
    /* pauseOnBlur */ false,
    /* locale */ "",
#if defined(_WIN32) || defined(FLATPAK_VERSION)
    /* discordIntegration */ true,
#else
    /* discordIntegration */ false,
#endif
    /* inputDriver */ InputDriverUtil::getDefault(),
#ifdef _WIN32
    /* sdlRawInput */ false,
#endif
    /* hideRetroArchNotifications */ false,
    /* showAdvancedSettings */ false,
    /* knowsAboutRhdcView */ false,
    /* ignoreGamecubeWarnings */ false,
    /* fullscreen */ false,
#ifdef _WIN32
    /* audioDriver */ AudioDriver::DirectSound,
#endif
#if defined(__APPLE__)
    /* forceX86 */ false,
#endif
    /* enableIsViewer */ false,
    /* isViewerHistorySize */ 250,
    /* emulateSdCard */ false,
    /* rtcOffset */ 0,
    /* rtcSavestates */ false};

static constexpr char P_VISIBLE_COLUMNS[] = "visible_columns";
static constexpr char P_DEFAULT_PARALLEL_PLUGIN[] = "default_gfx_plugin";
static constexpr char P_DEFAULT_MUPEN_PLUGIN[] = "default_gfx_plugin_mupen";
static constexpr char P_DEFAULT_EMULATOR[] = "default_emulator";
static constexpr char P_PARALLEL_UPSCALING[] = "parallel_upscaling";
static constexpr char P_PARALLEL_AA[] = "parallel_antialiasing";
static constexpr char P_PARALLEL_UPSCALE_TEXRECT[] =
    "parallel_upscale_texrects";
static constexpr char P_PARALLEL_REMOVE_BORDERS[] = "parallel_remove_borders";
static constexpr char P_ANGRYLION_FILTERING[] = "angrylion_filtering";
static constexpr char P_GLIDEN_AA[] = "gliden64_antialiasing";
static constexpr char P_GLIDEN_3POINT[] = "gliden64_3point_filtering";
static constexpr char P_GLIDEN_FIX_DEPTH[] = "gliden64_correct_depth_compare";
static constexpr char P_GLIDEN_EMU_FRAMEBUF[] = "gliden64_emulate_framebuffer";
static constexpr char P_WINDOW_SCALE[] = "window_scale";
static constexpr char P_VSYNC[] = "enable_vsync";
static constexpr char P_HIDE_WHILE_PLAYING[] = "hide_while_playing";
static constexpr char P_PREFERRED_CONTROLLER[] = "preferred_controller";
#ifndef __linux__
static constexpr char P_THEME[] = "windows_theme"; // Also applies to MacOS
static constexpr char P_CHECK_FOR_UPDATES[] = "check_for_updates";
static constexpr char P_DARK_MODE[] = "dark_mode";
#endif
static constexpr char P_PATCH_TO_SAME_FOLDER[] = "patch_to_same_folder";
static constexpr char P_PATCHED_ROM_FOLDER[] = "patched_rom_folder";
static constexpr char P_MIGRATION_STATE[] = "migration_state";
static constexpr char P_CORE_UPDATE_INTERVAL[] = "core_update_interval";
static constexpr char P_MUPEN_DEV[] = "use_mupen_dev_branch";
static constexpr char P_PAUSE_ON_BLUR[] = "pause_on_focus_loss";
static constexpr char P_LOCALE[] = "locale";
static constexpr char P_DISCORD_INTEGRATION[] = "discord_integration";
static constexpr char P_INPUT_DRIVER[] = "input_driver";
#ifdef _WIN32
static constexpr char P_SDL_RAWINPUT[] = "sdl_rawinput";
#endif
static constexpr char P_HIDE_NOTIFICATIONS[] = "hide_retro_notifications";
static constexpr char P_SHOW_ADVANCED[] = "show_advanced_rom_settings";
static constexpr char P_SEEN_RHDC_VIEW[] = "knows_about_rhdc_view";
static constexpr char P_IGNORE_GC_WARN[] =
    "ignore_gamecube_controller_binding_warnings";
static constexpr char P_FULLSCREEN[] = "fullscreen";
#ifdef _WIN32
static constexpr char P_AUDIO_DRIVER[] = "audio_driver";
#endif
#if defined(__APPLE__)
static constexpr char P_FORCE_X86[] = "force_x86";
#endif
static constexpr char P_IS_VIEWER[] = "is_viewer";
static constexpr char P_IS_VIEWER_HISTORY[] = "is_viewer_history";
static constexpr char P_SD_CARD[] = "emulate_sd_card";
static constexpr char P_RTC_OFFSET[] = "rtc_offset";
static constexpr char P_RTC_SAVESTATE[] = "rtc_savestate";

template <>
void JsonSerializer::serialize<AppSettings>(JsonWriter &jw,
                                            const AppSettings &obj) {
  jw.writeObjectStart();
  jw.writeProperty(P_VISIBLE_COLUMNS, obj.visibleColumns);
  jw.writeProperty(P_DEFAULT_PARALLEL_PLUGIN, obj.defaultParallelPlugin);
  jw.writeProperty(P_DEFAULT_MUPEN_PLUGIN, obj.defaultMupenPlugin);
  jw.writeProperty(P_DEFAULT_EMULATOR, obj.defaultEmulator);
  jw.writeProperty(P_PARALLEL_UPSCALING, obj.parallelUpscaling);
  jw.writeProperty(P_PARALLEL_REMOVE_BORDERS, obj.parallelRemoveBorders);
  jw.writeProperty(P_PARALLEL_AA, obj.parallelAntiAliasing);
  jw.writeProperty(P_PARALLEL_UPSCALE_TEXRECT, obj.parallelTexRectUpscaling);
  jw.writeProperty(P_ANGRYLION_FILTERING, obj.angrylionFiltering);
  jw.writeProperty(P_GLIDEN_AA, obj.glidenAntiAliasing);
  jw.writeProperty(P_GLIDEN_3POINT, obj.glidenThreePointFiltering);
  jw.writeProperty(P_GLIDEN_FIX_DEPTH, obj.glidenCorrectDepthCompare);
  jw.writeProperty(P_GLIDEN_EMU_FRAMEBUF, obj.glidenFramebufferEmulation);
  jw.writeProperty(P_WINDOW_SCALE, obj.windowScale);
  jw.writeProperty(P_VSYNC, obj.vsync);
  jw.writeProperty(P_HIDE_WHILE_PLAYING, obj.hideWhenPlaying);
  jw.writePropertyName(P_PREFERRED_CONTROLLER);
  if (obj.preferredController.has_value()) {
    jw.writeString(obj.preferredController.value().toString());
  } else {
    jw.writeNull();
  }
#ifndef __linux__
  jw.writeProperty(P_THEME, obj.theme);
  jw.writeProperty(P_CHECK_FOR_UPDATES, obj.checkForUpdates);
  jw.writeProperty(P_DARK_MODE, obj.darkMode);
#endif
  jw.writeProperty(P_PATCH_TO_SAME_FOLDER, obj.patchToSameFolder);
  jw.writeProperty(P_PATCHED_ROM_FOLDER, obj.patchedRomFolder.u8string());
  jw.writeProperty(P_MIGRATION_STATE, AppSettings::Default.migrationState);
  jw.writeProperty(P_CORE_UPDATE_INTERVAL, obj.coreUpdateInterval);
  jw.writeProperty(P_MUPEN_DEV, obj.mupenDevBranch);
  jw.writeProperty(P_PAUSE_ON_BLUR, obj.pauseOnBlur);
  jw.writeProperty(P_LOCALE, obj.locale);
  jw.writeProperty(P_DISCORD_INTEGRATION, obj.discordIntegration);
  jw.writePropertyName(P_INPUT_DRIVER);
  JsonSerializer::serialize<InputDriver>(jw, obj.inputDriver);
#ifdef _WIN32
  jw.writeProperty(P_SDL_RAWINPUT, obj.sdlRawInput);
#endif
  jw.writeProperty(P_HIDE_NOTIFICATIONS, obj.hideRetroArchNotifications);
  jw.writeProperty(P_SHOW_ADVANCED, obj.showAdvancedSettings);
  jw.writeProperty(P_SEEN_RHDC_VIEW, obj.knowsAboutRhdcView);
  jw.writeProperty(P_IGNORE_GC_WARN, obj.ignoreGamecubeWarnings);
  jw.writeProperty(P_FULLSCREEN, obj.fullscreen);
#ifdef _WIN32
  jw.writeProperty(P_AUDIO_DRIVER, obj.audioDriver);
#endif
#if defined(__APPLE__)
  jw.writeProperty(P_FORCE_X86, obj.forceX86);
#endif
  jw.writeProperty(P_IS_VIEWER, obj.enableIsViewer);
  jw.writeProperty(P_IS_VIEWER_HISTORY, obj.isViewerHistorySize);
  jw.writeProperty(P_SD_CARD, obj.emulateSdCard);
  jw.writeProperty(P_RTC_OFFSET, obj.rtcOffset);
  jw.writeProperty(P_RTC_SAVESTATE, obj.rtcSavestates);
  jw.writeObjectEnd();
}

template <> AppSettings JsonSerializer::parse<AppSettings>(const Json &json) {
  return AppSettings {
    json[P_VISIBLE_COLUMNS].getOrDefault<RomInfoColumn>(
        AppSettings::Default.visibleColumns),
        json[P_DEFAULT_PARALLEL_PLUGIN].getOrDefault<GfxPlugin>(
            AppSettings::Default.defaultParallelPlugin),
        json[P_DEFAULT_MUPEN_PLUGIN].getOrDefault<GfxPlugin>(
            AppSettings::Default.defaultMupenPlugin),
        json[P_DEFAULT_EMULATOR].getOrDefault<EmulatorCore>(
            AppSettings::Default.defaultEmulator),
        json[P_PARALLEL_UPSCALING].getOrDefault<ParallelUpscaling>(
            AppSettings::Default.parallelUpscaling),
        json[P_PARALLEL_AA].getOrDefault<bool>(
            AppSettings::Default.parallelAntiAliasing),
        json[P_PARALLEL_UPSCALE_TEXRECT].getOrDefault<bool>(
            AppSettings::Default.parallelTexRectUpscaling),
        json[P_PARALLEL_REMOVE_BORDERS].getOrDefault<bool>(
            AppSettings::Default.parallelRemoveBorders),
        json[P_ANGRYLION_FILTERING].getOrDefault<AngrylionFiltering>(
            AppSettings::Default.angrylionFiltering),
        json[P_GLIDEN_AA].getOrDefault<bool>(
            AppSettings::Default.glidenAntiAliasing),
        json[P_GLIDEN_3POINT].getOrDefault<bool>(
            AppSettings::Default.glidenThreePointFiltering),
        json[P_GLIDEN_FIX_DEPTH].getOrDefault<bool>(
            AppSettings::Default.glidenCorrectDepthCompare),
        json[P_GLIDEN_EMU_FRAMEBUF].getOrDefault<bool>(
            AppSettings::Default.glidenFramebufferEmulation),
        SteamDeck::isSteamDeck() ? (ubyte)4
                                 : json[P_WINDOW_SCALE].getOrDefault<ubyte>(
                                       AppSettings::Default.windowScale),
        json[P_VSYNC].getOrDefault<bool>(AppSettings::Default.vsync),
        SteamDeck::isSteamDeck()
            ? false
            : json[P_HIDE_WHILE_PLAYING].getOrDefault<bool>(
                  AppSettings::Default.hideWhenPlaying),
        (json[P_PREFERRED_CONTROLLER].exists() &&
         !json[P_PREFERRED_CONTROLLER].isNull())
            ? Uuid::parse(json[P_PREFERRED_CONTROLLER].get<string>())
            : std::optional<Uuid>(),
#ifndef __linux__
        json[P_THEME].getOrDefault<string>(AppSettings::Default.theme),
        json[P_CHECK_FOR_UPDATES].getOrDefault<bool>(
            AppSettings::Default.checkForUpdates),
        json[P_DARK_MODE].getOrDefault<bool>(AppSettings::Default.darkMode),
#endif
        json[P_PATCH_TO_SAME_FOLDER].getOrDefault<bool>(
            AppSettings::Default.patchToSameFolder),
        fs::to_path(json[P_PATCHED_ROM_FOLDER].getOrDefault<string>(
            AppSettings::Default.patchedRomFolder.u8string())),
        json[P_MIGRATION_STATE].getOrDefault<short>(0),
        json[P_CORE_UPDATE_INTERVAL].getOrDefault<CoreUpdateInterval>(
            AppSettings::Default.coreUpdateInterval),
        json[P_MUPEN_DEV].getOrDefault<bool>(
            AppSettings::Default.mupenDevBranch),
        json[P_PAUSE_ON_BLUR].getOrDefault<bool>(
            AppSettings::Default.pauseOnBlur),
        json[P_LOCALE].getOrDefault<string>(AppSettings::Default.locale),
        json[P_DISCORD_INTEGRATION].getOrDefault<bool>(
            AppSettings::Default.discordIntegration),
        JsonSerializer::parse<InputDriver>(json[P_INPUT_DRIVER]),
#ifdef _WIN32
        json[P_SDL_RAWINPUT].getOrDefault<bool>(
            AppSettings::Default.sdlRawInput),
#endif
        json[P_HIDE_NOTIFICATIONS].getOrDefault<bool>(
            AppSettings::Default.hideRetroArchNotifications),
        json[P_SHOW_ADVANCED].getOrDefault<bool>(
            AppSettings::Default.showAdvancedSettings),
        json[P_SEEN_RHDC_VIEW].getOrDefault<bool>(
            AppSettings::Default.knowsAboutRhdcView),
        json[P_IGNORE_GC_WARN].getOrDefault<bool>(
            AppSettings::Default.ignoreGamecubeWarnings),
        SteamDeck::isSteamDeck() ? true
                                 : json[P_FULLSCREEN].getOrDefault<bool>(
                                       AppSettings::Default.fullscreen),
#ifdef _WIN32
        json[P_AUDIO_DRIVER].getOrDefault<AudioDriver>(
            AppSettings::Default.audioDriver),
#endif
#if defined(__APPLE__)
        json[P_FORCE_X86].getOrDefault<bool>(AppSettings::Default.forceX86),
#endif
        json[P_IS_VIEWER].getOrDefault<bool>(
            AppSettings::Default.enableIsViewer),
        json[P_IS_VIEWER_HISTORY].getOrDefault<int>(
            AppSettings::Default.isViewerHistorySize),
        json[P_SD_CARD].getOrDefault<bool>(AppSettings::Default.emulateSdCard),
        json[P_RTC_OFFSET].getOrDefault<int>(AppSettings::Default.rtcOffset),
        json[P_RTC_SAVESTATE].getOrDefault<bool>(
            AppSettings::Default.rtcSavestates)
  };
}

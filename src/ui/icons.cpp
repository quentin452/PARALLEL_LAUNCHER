#include "src/ui/icons.hpp"

#ifdef __linux__
#include "src/ui/palette.hpp"
#else
#include "src/core/file-controller.hpp"
#endif
#include "src/types.hpp"

static inline QIcon getFallbackIcon(const char *filename) {
#ifdef __linux__
  return ColourPalette::systemIsDarkMode()
             ?
#else
  return FileController::loadAppSettings().darkMode
             ?
#endif
             QIcon((":/fallback-icons/dark/"s + filename).c_str())
             : QIcon((":/fallback-icons/light/"s + filename).c_str());
}

#define LOAD_ICON(varName, iconName)                                           \
  const QIcon &Icon::varName() {                                               \
    static const QIcon s_icon =                                                \
        QIcon::fromTheme(iconName, getFallbackIcon(iconName ".svg"));          \
    return s_icon;                                                             \
  }

#define LOAD_COLOUR_ICON(varName, iconName)                                    \
  const QIcon &Icon::varName() {                                               \
    static const QIcon s_icon = QIcon::fromTheme(                              \
        iconName, QIcon(":/fallback-icons/common/" iconName ".svg"));          \
    return s_icon;                                                             \
  }

#define LOAD_SYMBOL_ICON(varName, iconName)                                    \
  const QIcon &Icon::varName() {                                               \
    static const QIcon s_icon = QIcon(":/symbol/" iconName ".svg");            \
    return s_icon;                                                             \
  }

const QIcon &Icon::appIcon() {
  static const QIcon s_icon = QIcon(":/appicon.svg");
  return s_icon;
}

LOAD_ICON(menu, "application-menu")
LOAD_COLOUR_ICON(delet, "delete")
LOAD_ICON(cancel, "dialog-cancel")
LOAD_COLOUR_ICON(close, "dialog-close")
LOAD_COLOUR_ICON(dialogError, "dialog-error")
LOAD_COLOUR_ICON(dialogInfo, "dialog-information")
LOAD_ICON(ok, "dialog-ok")
LOAD_COLOUR_ICON(dialogWarning, "dialog-warning")
LOAD_ICON(browse, "document-open")
LOAD_ICON(save, "document-save")
LOAD_ICON(saveAs, "document-save-as")
LOAD_ICON(saveAll, "document-save-all")
LOAD_ICON(edit, "document-edit")
LOAD_ICON(gamepad, "input-gamepad")
LOAD_ICON(add, "list-add")
LOAD_ICON(play, "media-playback-start")
LOAD_ICON(fastForward, "media-seek-forward")
LOAD_COLOUR_ICON(pkill, "process-stop")
LOAD_ICON(refresh, "view-refresh")
LOAD_ICON(skip, "go-next-skip")
LOAD_ICON(group, "group")
LOAD_ICON(configure, "configure")
LOAD_ICON(clear, "edit-clear")
LOAD_ICON(search, "edit-find")
LOAD_ICON(keyboard, "input-keyboard")
LOAD_ICON(undo, "edit-undo")
LOAD_ICON(login, "user")
LOAD_ICON(logout, "system-shutdown")
LOAD_ICON(plugin, "plugins")
LOAD_ICON(download, "download")
LOAD_ICON(sortAsc, "view-sort-ascending")
LOAD_ICON(sortDsc, "view-sort-descending")
LOAD_COLOUR_ICON(recycle, "application-x-trash")
LOAD_ICON(mount, "media-mount")
LOAD_ICON(eject, "media-eject")
LOAD_ICON(shuffle, "media-playlist-shuffle")
LOAD_ICON(clone, "edit-copy")
LOAD_SYMBOL_ICON(ratingDisliked, "rating-disliked")
LOAD_SYMBOL_ICON(ratingNeutral, "rating-neutral")
LOAD_SYMBOL_ICON(ratingLiked, "rating-liked")
LOAD_SYMBOL_ICON(ratingLoved, "rating-loved")
LOAD_SYMBOL_ICON(ratingDislikedFilled, "rating-disliked-filled")
LOAD_SYMBOL_ICON(ratingNeutralFilled, "rating-neutral-filled")
LOAD_SYMBOL_ICON(ratingLikedFilled, "rating-liked-filled")
LOAD_SYMBOL_ICON(ratingLovedFilled, "rating-loved-filled")
LOAD_SYMBOL_ICON(rhdcLight, "rhdc-logo-light")
LOAD_SYMBOL_ICON(rhdcDark, "rhdc-logo-dark")
LOAD_SYMBOL_ICON(rhdcLink, "rhdc-link")

#undef LOAD_ICON

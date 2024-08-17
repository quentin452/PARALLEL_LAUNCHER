#ifndef SRC_UI_NOW_PLAYING_WINDOW_HPP_
#define SRC_UI_NOW_PLAYING_WINDOW_HPP_

#include "src/core/filesystem.hpp"
#include "src/polyfill/process.hpp"
#include "src/rhdc/core/layout.hpp"

namespace NowPlayingWindow {

extern void open(const fs::path &romPath,
                 const string &sha1, int64 pastPlayTime,
                 const StarLayout *layoutOverride);

extern void close();
extern void free();

extern bool shouldReload();

} // namespace NowPlayingWindow

#endif /* SRC_UI_NOW_PLAYING_WINDOW_HPP_ */

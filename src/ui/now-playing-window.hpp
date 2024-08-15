#ifndef SRC_UI_NOW_PLAYING_WINDOW_HPP_
#define SRC_UI_NOW_PLAYING_WINDOW_HPP_

#include "src/polyfill/process.hpp"
#include "src/core/filesystem.hpp"
#include "src/rhdc/core/layout.hpp"

namespace NowPlayingWindow {

	extern void open(
		AsyncProcess *process,
		const fs::path &romPath,
		const string &sha1,
		int64 pastPlayTime,
		const StarLayout *layoutOverride
	);

	extern void close();
	extern void free();

	extern bool shouldReload();

}



#endif /* SRC_UI_NOW_PLAYING_WINDOW_HPP_ */

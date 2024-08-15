#ifndef SRC_UI_PLAY_HPP_
#define SRC_UI_PLAY_HPP_

#include <functional>
#include "src/core/rom.hpp"
#include "src/rhdc/core/layout.hpp"

namespace Game {

	extern bool tryLoadRom(
		fs::path romPath,
		/* out */ RomInfo &romInfo,
		/* out */ RomFile &romFile,
		/* out */ bool &firstRun
	);

	extern bool play(
		const RomFile &romFile,
		const RomInfo &romInfo,
		bool multiplayer,
		bool waitForRhdcSync,
		const std::function<void()> &callback,
		const StarLayout *testLayout = nullptr
	);

}



#endif /* SRC_UI_PLAY_HPP_ */

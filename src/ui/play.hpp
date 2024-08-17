#ifndef SRC_UI_PLAY_HPP_
#define SRC_UI_PLAY_HPP_

#include "src/core/rom.hpp"
#include "src/rhdc/core/layout.hpp"
#include <functional>

namespace Game {
extern bool play(const RomFile &romFile, const RomInfo &romInfo,
                 bool multiplayer, bool waitForRhdcSync,
                 const std::function<void()> &callback,
                 const StarLayout *testLayout = nullptr);

} // namespace Game

#endif /* SRC_UI_PLAY_HPP_ */

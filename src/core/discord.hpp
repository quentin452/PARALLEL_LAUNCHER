#ifndef SRC_CORE_DISCORD_HPP_
#define SRC_CORE_DISCORD_HPP_

#include "src/types.hpp"

namespace Discord {

bool pluginInstalled();
void setNowPlaying(const string &gameName);
void setNowPlaying(const string &gameName, int64 startTime);
inline void clearNowPlaying() { setNowPlaying(""); }

} // namespace Discord

#endif /* SRC_CORE_DISCORD_HPP_ */

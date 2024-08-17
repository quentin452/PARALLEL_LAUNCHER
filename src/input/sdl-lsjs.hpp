#if !defined(SRC_INPUT_SDL_LSJS_HPP_) && !defined(__APPLE__)
#define SRC_INPUT_SDL_LSJS_HPP_

#include "src/core/uuid.hpp"
#include <vector>

namespace lsjs {

std::vector<Uuid> tryGetPortOrder(bool &success);

}

#endif /* SRC_INPUT_SDL_LSJS_HPP_ */

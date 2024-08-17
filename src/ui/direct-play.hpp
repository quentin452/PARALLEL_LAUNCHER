#ifndef SRC_CORE_DIRECT_PLAY_HPP_
#define SRC_CORE_DIRECT_PLAY_HPP_

#include "src/core/filesystem.hpp"
#include <QApplication>

namespace DirectPlay {
extern int start(QApplication &app, fs::path romPath);
}

#endif /* SRC_CORE_DIRECT_PLAY_HPP_ */

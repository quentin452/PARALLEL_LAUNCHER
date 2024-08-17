#ifndef SRC_RHDC_CORE_SYNC_HPP_
#define SRC_RHDC_CORE_SYNC_HPP_

#include "src/core/filesystem.hpp"
#include <functional>

namespace RHDC {

extern void sync(const std::function<void(bool)> &callback);
extern void moveRhdcFolder(const fs::path &oldDir, const fs::path &newDir);

} // namespace RHDC

#endif /* SRC_RHDC_CORE_SYNC_HPP_ */

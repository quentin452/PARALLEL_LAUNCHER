#ifndef SRC_INPUT_INPUT_DRIVER_HPP_
#define SRC_INPUT_INPUT_DRIVER_HPP_

#include "src/core/json.hpp"
#include <set>

enum class InputDriver : int {
  HID,
  SDL,
  DirectInput,
  JoyDev,
};

namespace InputDriverUtil {

extern InputDriver getDefault();
extern const std::set<InputDriver> &getSupported();

inline bool isSupported(InputDriver driver) {
  const std::set<InputDriver> &supported = getSupported();
  return supported.find(driver) != supported.end();
}

} // namespace InputDriverUtil

namespace JsonSerializer {
template <> void serialize<InputDriver>(JsonWriter &jw, const InputDriver &obj);
template <> InputDriver parse<InputDriver>(const Json &json);
} // namespace JsonSerializer

#endif /* SRC_INPUT_INPUT_DRIVER_HPP_ */

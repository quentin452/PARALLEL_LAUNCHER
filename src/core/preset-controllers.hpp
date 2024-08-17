#ifndef SRC_CORE_PRESET_CONTROLLERS_HPP_
#define SRC_CORE_PRESET_CONTROLLERS_HPP_

#include "src/core/controller.hpp"
#include "src/input/input-driver.hpp"
#include <vector>

enum class ControllerType { Gamecube, XBox360, Nintendo64, SwitchPro, Other };

ControllerType getControllerType(const ControllerId &controllerId);

namespace DefaultProfile {
extern const std::vector<ControllerProfile> &getAll(InputDriver driver);
extern const ControllerProfile &
get(InputDriver driver, ControllerType type = ControllerType::XBox360);
extern bool exists(InputDriver driver, const string &name);
} // namespace DefaultProfile

namespace DefaultInputModes {
extern const InputMode Normal;
extern const InputMode DualAnalog;
extern const InputMode GoldenEye;
extern const InputMode Clone;

extern bool exists(const Uuid &uuid);
} // namespace DefaultInputModes

#endif /* SRC_CORE_PRESET_CONTROLLERS_HPP_ */

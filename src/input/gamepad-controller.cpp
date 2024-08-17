#include "src/input/gamepad-controller.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/input/gamepad-input-driver.hpp"
#include <memory>
#include <unordered_map>

#if defined(__linux__)
#include "src/input/common/sdl-driver.hpp"
#include "src/input/linux/joydev-driver.hpp"
#elif defined(_WIN32)
#include "src/input/common/sdl-driver.hpp"
#include "src/input/windows/direct-input-driver.hpp"
#elif defined(__APPLE__)
#include "src/input/macos/hid-driver.hpp"
#endif

static std::vector<std::unique_ptr<IGamepadDriver>> s_drivers;
static uint_fast8_t s_driverIndex = 0;

#define s_currentDriver s_drivers[s_driverIndex]
#define REGISTER_DRIVER(driverClass)                                           \
  s_drivers.push_back(                                                         \
      std::unique_ptr<IGamepadDriver>(new driverClass(eventEmitter)));

const HashMap<ControllerType, const char *> s_knownNames = {
    {ControllerType::Gamecube, "Gamecube Controller"},
    {ControllerType::XBox360, "XBox 360 Controller"},
    {ControllerType::Nintendo64, "Nintendo 64 Controller"}};

GamepadController::GamepadController() : QObject() {
  const GamepadEventEmitter eventEmitter = {
      GamepadController::onGamepadConnected,
      GamepadController::onGamepadDisconnected,
      GamepadController::onGamepadButtonPressed,
      GamepadController::onGamepadAxisMoved,
      GamepadController::onGamepadHatMoved};

#if defined(__linux__)
  REGISTER_DRIVER(SdlDriver)
  REGISTER_DRIVER(JoyDevDriver)
#elif defined(_WIN32)
  REGISTER_DRIVER(SdlDriver)
  REGISTER_DRIVER(DirectInputDriver)
#elif defined(__APPLE__)
  REGISTER_DRIVER(HidDriver)
#else
  static_assert(false);
#endif

  setInputDriver(FileController::loadAppSettings().inputDriver);
}

GamepadController::~GamepadController() { stop(); }

void GamepadController::start() { s_currentDriver->start(); }

void GamepadController::stop() { s_currentDriver->stop(); }

void GamepadController::setInputDriver(InputDriver driver) {
  for (size_t i = 0; i < s_drivers.size(); i++) {
    if (s_drivers[i]->id() == driver) {
      const bool wasRunning = s_currentDriver->isRunning();
      stop();
      s_driverIndex = (uint_fast8_t)i;
      if (wasRunning) {
        start();
      }
      return;
    }
  }
}

std::vector<ConnectedGamepad> GamepadController::getConnected() {
  return s_currentDriver->getConnected();
}

GamepadState GamepadController::getState(GamepadId id) {
  return s_currentDriver->getState(id);
}

std::vector<Uuid> GamepadController::getDeviceOrder() {
  return s_currentDriver->getDeviceOrder();
}

GamepadController &GamepadController::instance() {
  static GamepadController s_this;
  return s_this;
}

void GamepadController::onGamepadConnected(GamepadConnectedEvent &event) {
  if (event.info.controllerId.vendorId == 0x28DE &&
      event.info.controllerId.productId == 0x11FF) {
    event.info.name = "Steam Deck Controller";
  } else if (event.info.name.empty()) {
    auto i = s_knownNames.find(getControllerType(event.info.controllerId));
    event.info.name = (i != s_knownNames.end()) ? i->second : "";
  } else if (event.info.controllerId.vendorId == 0 &&
             event.info.controllerId.productId == 0 &&
             event.info.name == "Wii U GameCube Adapter") {
    event.info.controllerId.vendorId = 0x057E;
    event.info.controllerId.productId = 0x0337;
  }
  instance().fireGamepadConnected(event);
}

void GamepadController::onGamepadDisconnected(
    const GamepadDisconnectedEvent &event) {
  instance().fireGamepadDisconnected(event);
}

void GamepadController::onGamepadButtonPressed(
    const GamepadButtonEvent &event) {
  instance().fireGamepadButtonPressed(event);
}

void GamepadController::onGamepadAxisMoved(const GamepadAxisEvent &event) {
  instance().fireGamepadAxisMoved(event);
}

void GamepadController::onGamepadHatMoved(const GamepadHatEvent &event) {
  instance().fireGamepadHatMoved(event);
}

#undef s_currentDriver
#undef REGISTER_DRIVER

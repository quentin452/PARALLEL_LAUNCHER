#ifndef SRC_INPUT_WINDOWS_DIRECT_INPUT_DRIVER_HPP_
#define SRC_INPUT_WINDOWS_DIRECT_INPUT_DRIVER_HPP_

#include "src/input/gamepad-input-driver.hpp"

class DirectInputDriver : public IGamepadDriver {

public:
  DirectInputDriver(const GamepadEventEmitter &eventEmitter);
  ~DirectInputDriver();

  InputDriver id() const override { return InputDriver::DirectInput; }

  void start() override;
  void stop() override;
  bool isRunning() const override;

  std::vector<ConnectedGamepad> getConnected() override;
  std::vector<Uuid> getDeviceOrder() override;
  GamepadState getState(GamepadId id) const override;

private:
  void processEvents();
};

#endif /* SRC_INPUT_WINDOWS_DIRECT_INPUT_DRIVER_HPP_ */

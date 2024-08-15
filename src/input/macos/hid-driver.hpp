#ifndef SRC_INPUT_MACOS_HID_DRIVER_HPP_
#define SRC_INPUT_MACOS_HID_DRIVER_HPP_

#include "src/input/gamepad-input-driver.hpp"

class HidDriver : public IGamepadDriver {

	public:
	HidDriver( const GamepadEventEmitter &eventEmitter );
	~HidDriver();

	InputDriver id() const override { return InputDriver::HID; }

	void start() override;
	void stop() override;
	bool isRunning() const override;

	std::vector<ConnectedGamepad> getConnected() override;
	GamepadState getState( GamepadId id ) const override;
	std::vector<Uuid> getDeviceOrder() override;

};

#endif /* SRC_INPUT_MACOS_HID_DRIVER_HPP_ */

#ifndef SRC_INPUT_LINUX_JOYDEV_DRIVER_HPP_
#define SRC_INPUT_LINUX_JOYDEV_DRIVER_HPP_

#include "src/input/gamepad-input-driver.hpp"

class JoyDevDriver : public IGamepadDriver {

	public:
	JoyDevDriver( const GamepadEventEmitter &eventEmitter );
	~JoyDevDriver();

	InputDriver id() const override { return InputDriver::JoyDev; }

	void start() override;
	void stop() override;
	bool isRunning() const override;

	std::vector<ConnectedGamepad> getConnected() override;
	std::vector<Uuid> getDeviceOrder() override;
	GamepadState getState( GamepadId id ) const override;

	private:
	void loadConnectedDevices( bool sendEvents ) const;
	void processEvents() const;

};


#endif /* SRC_INPUT_LINUX_JOYDEV_DRIVER_HPP_ */

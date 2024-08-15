#ifndef SRC_INPUT_GAMEPAD_INPUT_DRIVER_HPP_
#define SRC_INPUT_GAMEPAD_INPUT_DRIVER_HPP_

#include "src/input/gamepad-controller.hpp"

struct GamepadEventEmitter {
	void(*gamepadConnected)(GamepadConnectedEvent&);
	void(*gamepadDisconnected)(const GamepadDisconnectedEvent&);
	void(*gamepadButtonPressed)(const GamepadButtonEvent&);
	void(*gamepadAxisMoved)(const GamepadAxisEvent&);
	void(*gamepadHatMoved)(const GamepadHatEvent&);
};

class IGamepadDriver {
	
	protected:
	const GamepadEventEmitter m_events;
	
	IGamepadDriver( const GamepadEventEmitter &eventEmitter ) : m_events( eventEmitter ) {}
	
	public:
	virtual ~IGamepadDriver() {}
	
	virtual InputDriver id() const = 0;
	
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual bool isRunning() const = 0;
	
	virtual std::vector<ConnectedGamepad> getConnected() = 0;
	virtual std::vector<Uuid> getDeviceOrder() = 0;
	virtual GamepadState getState( GamepadId id ) const = 0;

};

#endif /* SRC_INPUT_GAMEPAD_INPUT_DRIVER_HPP_ */


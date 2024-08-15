#ifndef SRC_INPUT_JOYPAD_CONTROLLER_HPP_
#define SRC_INPUT_JOYPAD_CONTROLLER_HPP_

#include <QObject>
#include <vector>
#include "src/core/controller.hpp"
#include "src/input/input-driver.hpp"

typedef int GamepadId;

struct GamepadConnectedEvent {
	GamepadId id;
	ControllerInfo info;
};

struct GamepadDisconnectedEvent {
	GamepadId id;
};

struct GamepadButtonEvent {
	GamepadId id;
	ubyte button;
	bool isPressed;
};

struct GamepadAxisEvent {
	GamepadId id;
	ubyte axis;
	double position;
};

struct GamepadHatEvent {
	GamepadId id;
	ubyte hat;
	HatDirection position;
};

struct GamepadState {
	std::vector<bool> buttons;
	std::vector<double> axes;
	std::vector<HatDirection> hats;
};

struct ConnectedGamepad {
	GamepadId id;
	ControllerInfo info;
};

class GamepadController : public QObject {
	Q_OBJECT

	private:
	GamepadController();
	
	public:
	~GamepadController();

	void start();
	void stop();
	
	void setInputDriver( InputDriver driver );

	static std::vector<ConnectedGamepad> getConnected();
	static GamepadState getState( GamepadId id );
	static std::vector<Uuid> getDeviceOrder();

	static GamepadController &instance();
	
	signals:
	void gamepadConnected( GamepadConnectedEvent );
	void gamepadDisconnected( GamepadDisconnectedEvent );
	void gamepadButtonPressed( GamepadButtonEvent );
	void gamepadAxisMoved( GamepadAxisEvent );
	void gamepadHatMoved( GamepadHatEvent );
	
	private:
	inline void fireGamepadConnected( const GamepadConnectedEvent &event ) { emit gamepadConnected( event ); }
	inline void fireGamepadDisconnected( const GamepadDisconnectedEvent &event ) { emit gamepadDisconnected( event ); }
	inline void fireGamepadButtonPressed( const GamepadButtonEvent &event ) { emit gamepadButtonPressed( event ); }
	inline void fireGamepadAxisMoved( const GamepadAxisEvent &event ) { emit gamepadAxisMoved( event ); }
	inline void fireGamepadHatMoved( const GamepadHatEvent &event ) { emit gamepadHatMoved( event ); }
		
	static void onGamepadConnected( GamepadConnectedEvent &event );
	static void onGamepadDisconnected( const GamepadDisconnectedEvent &event );
	static void onGamepadButtonPressed( const GamepadButtonEvent &event );
	static void onGamepadAxisMoved( const GamepadAxisEvent &event );
	static void onGamepadHatMoved( const GamepadHatEvent &event );

};

#endif /* SRC_INPUT_JOYPAD_CONTROLLER_HPP_ */

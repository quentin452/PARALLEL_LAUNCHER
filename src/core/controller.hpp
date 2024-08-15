#ifndef SRC_CORE_CONTROLLER_HPP_
#define SRC_CORE_CONTROLLER_HPP_

#include <array>
#include <functional>
#include "src/types.hpp"
#include "src/core/uuid.hpp"
#include "src/core/json.hpp"
#include "src/core/flags.hpp"

enum class ControllerAction : ubyte {
	AnalogUp = 0,
	AnalogDown,
	AnalogLeft,
	AnalogRight,
	CUp,
	CDown,
	CLeft,
	CRight,
	DPadUp,
	DPadDown,
	DPadLeft,
	DPadRight,
	ButtonA,
	ButtonB,
	TriggerL,
	TriggerZ,
	TriggerR,
	Start,
	ButtonX,
	ButtonY,

	NUM_INPUTS,

	SaveState = NUM_INPUTS,
	LoadState,
	FastForward,
	SlowMotion,

	NUM_ACTIONS,
	None = 0x99
};

typedef std::array<ControllerAction,(size_t)ControllerAction::NUM_INPUTS> InputMapping;

struct InputMode {
	Uuid id;
	string name;
	string description;
	InputMapping port1;
	InputMapping port2;

	bool usesTwoPorts() const;
};

enum class BindingType : ubyte {
	None = 0,
	Button = 2,
	AxisPositive = 3,
	AxisNegative = 4,
	HatUp = 5,
	HatDown = 6,
	HatLeft = 7,
	HatRight = 8
};

enum class HatDirection : ubyte {
	Centre = 0,
	Up = 1,
	Down = 4,
	Left = 8,
	Right = 2
};
DEFINE_FLAG_OPERATIONS( HatDirection, ubyte );

struct Binding {
	BindingType type;
	ushort buttonOrAxis;
};

struct ControllerId {
	ushort vendorId;
	ushort productId;

	inline bool operator==( const ControllerId &other ) const noexcept {
		return productId == other.productId && vendorId == other.vendorId;
	}

	inline bool operator!=( const ControllerId &other ) const noexcept {
		return productId != other.productId || vendorId != other.vendorId;
	}
};

template<> struct std::hash<ControllerId> {
	inline size_t operator()(const ControllerId &id) const noexcept {
		return ((size_t)id.vendorId << 16) | (size_t)id.productId;
	}
};

template<> struct std::hash<Binding> {
	inline size_t operator()(const Binding &binding) const noexcept {
		return ((size_t)binding.type << 16) | (size_t)binding.buttonOrAxis;
	}
};

struct ControllerInfo {
	string name;
	ControllerId controllerId;
	Uuid uuid;

	ushort numButtons;
	ushort numAxes;
	ushort numHats;
};

struct ControllerProfile {
	string name;
	Binding bindings[(ubyte)ControllerAction::NUM_ACTIONS];
	double sensitivity;
	double deadzone;
	double axisThreshold;
	bool rumble;
};

struct PlayerController {
	ControllerProfile profile;
	Uuid deviceUuid;
};

namespace JsonSerializer {
	template<> void serialize<Binding>( JsonWriter &jw, const Binding &obj );
	template<> Binding parse<Binding>( const Json &json );

	template<> void serialize<ControllerProfile>( JsonWriter &jw, const ControllerProfile &obj );
	template<> ControllerProfile parse<ControllerProfile>( const Json &json );

	template<> void serialize<InputMode>( JsonWriter &jw, const InputMode &obj );
	template<> InputMode parse<InputMode>( const Json &json );
}

#endif /* SRC_CORE_CONTROLLER_HPP_ */

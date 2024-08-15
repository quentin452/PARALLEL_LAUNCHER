#ifndef SRC_CORE_HOTKEYS_HPP_
#define SRC_CORE_HOTKEYS_HPP_

#include <array>
#include "src/types.hpp"
#include "src/core/controller.hpp"
#include "src/input/keyboard.hpp"

enum class Hotkey : ubyte {
	SaveState = (ubyte)ControllerAction::SaveState,
	LoadState,
	PrevState,
	NextState,
	PrevCheat,
	NextCheat,
	ToggleCheat,
	ToggleFps,
	Pause,
	FrameAdvance,
	FastForward,
	ToggleFastForward,
	SlowMotion,
	ToggleSlowMotion,
	Rewind,
	Quit,
	HardReset,
	ToggleFullscreen,
	RetroMenu,
	RecordVideo,
	RecordInputs,
	Screenshot,
	VolumeUp,
	VolumeDown,
	Mute,
	GrabMouse,

	NUM_HOTKEYS
};

namespace Hotkeys {
	extern const KeyId Default[(ubyte)Hotkey::NUM_HOTKEYS];
	extern const char *const ConfigNames[(ubyte)Hotkey::NUM_HOTKEYS];
}

#endif /* SRC_CORE_HOTKEYS_HPP_ */

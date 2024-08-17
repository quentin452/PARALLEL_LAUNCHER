#ifndef SRC_INPUT_KEYBOARD_HPP_
#define SRC_INPUT_KEYBOARD_HPP_

#include "src/types.hpp"

struct KeyName {
  const char *retroConfigName;
  const char *displayName;
};

enum class KeyId : uint {
  Num0 = 0,
  Num1,
  Num2,
  Num3,
  Num4,
  Num5,
  Num6,
  Num7,
  Num8,
  Num9,
  Numpad0,
  Numpad1,
  Numpad2,
  Numpad3,
  Numpad4,
  Numpad5,
  Numpad6,
  Numpad7,
  Numpad8,
  Numpad9,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F20,
  F21,
  F22,
  F23,
  F24,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  Grave,
  Hyphen,
  Equals,
  LeftBracket,
  RightBracket,
  Backslash,
  Semicolon,
  Quote,
  Comma,
  Period,
  Slash,
  NumpadSlash,
  NumpadAsterisk,
  NumpadMinus,
  NumpadPlus,
  NumpadEnter,
  NumpadPeriod,
  NumLock,
  ArrowUp,
  ArrowLeft,
  ArrowDown,
  ArrowRight,
  PrintScreen,
  ScrollLock,
  Break,
  Insert,
  Delete,
  Home,
  End,
  PageUp,
  PageDown,
  Backspace,
  Tab,
  CapsLock,
  Enter,
  LeftShift,
  RightShift,
  LeftControl,
  RightControl,
  LeftAlt,
  RightAlt,
  LeftSuper,
  RightSuper,
  Space,
  Menu,
  Escape,

  LAST_VALID = Escape,
  NUM_KEYS,
  INVALID = 0xFF
};

namespace Keycode {
extern KeyId fromNativeCode(uint nativeKeyCode);
extern const KeyName &getNames(KeyId keyId);
} // namespace Keycode

#endif /* SRC_INPUT_KEYBOARD_HPP_ */

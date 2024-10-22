#include "src/input/keyboard.hpp"

static const KeyName s_unknownKeyName = {"nul", ""};

static const KeyName s_keyNames[(uint)KeyId::NUM_KEYS] = {
    {"num0", "0"},
    {"num1", "1"},
    {"num2", "2"},
    {"num3", "3"},
    {"num4", "4"},
    {"num5", "5"},
    {"num6", "6"},
    {"num7", "7"},
    {"num8", "8"},
    {"num9", "9"},
    {"keypad0", "Numpad 0"},
    {"keypad1", "Numpad 1"},
    {"keypad2", "Numpad 2"},
    {"keypad3", "Numpad 3"},
    {"keypad4", "Numpad 4"},
    {"keypad5", "Numpad 5"},
    {"keypad6", "Numpad 6"},
    {"keypad7", "Numpad 7"},
    {"keypad8", "Numpad 8"},
    {"keypad9", "Numpad 9"},
    {"f1", "F1"},
    {"f2", "F2"},
    {"f3", "F3"},
    {"f4", "F4"},
    {"f5", "F5"},
    {"f6", "F6"},
    {"f7", "F7"},
    {"f8", "F8"},
    {"f9", "F9"},
    {"f10", "F10"},
    {"f11", "F11"},
    {"f12", "F12"},
    {"f13", "F13"},
    {"f14", "F14"},
    {"f15", "F15"},
    {"f16", "F16"},
    {"f17", "F17"},
    {"f18", "F18"},
    {"f19", "F19"},
    {"f20", "F20"},
    {"f21", "F21"},
    {"f22", "F22"},
    {"f23", "F23"},
    {"f24", "F24"},
    {"a", "A"},
    {"b", "B"},
    {"c", "C"},
    {"d", "D"},
    {"e", "E"},
    {"f", "F"},
    {"g", "G"},
    {"h", "H"},
    {"i", "I"},
    {"j", "J"},
    {"k", "K"},
    {"l", "L"},
    {"m", "M"},
    {"n", "N"},
    {"o", "O"},
    {"p", "P"},
    {"q", "Q"},
    {"r", "R"},
    {"s", "S"},
    {"t", "T"},
    {"u", "U"},
    {"v", "V"},
    {"w", "W"},
    {"x", "X"},
    {"y", "Y"},
    {"z", "Z"},
    {"backquote", "`"},
    {"minus", "-"},
    {"equals", "="},
    {"leftbracket", "["},
    {"rightbracket", "]"},
    {"backslash", "\\"},
    {"semicolon", ";"},
    {"quote", "'"},
    {"comma", ","},
    {"period", "."},
    {"slash", "/"},
    {"divide", "Numpad /"},
    {"multiply", "Numpad *"},
    {"subtract", "Numpad -"},
    {"add", "Numpad +"},
    {"kp_enter", "Numpad Enter"},
    {"kp_period", "Numpad ."},
    {"numlock", "Num Lock"},
    {"up", "↑"},
    {"left", "←"},
    {"down", "↓"},
    {"right", "→"},
#ifdef __APPLE__
    {"sysreq", "F13 / Print Screen"},
#else
    {"sysreq", "Print Screen"},
#endif
    {"scroll_lock", "Scroll Lock"},
    {"pause", "Break"},
    {"insert", "Insert"},
    {"del", "Delete"},
    {"home", "Home"},
    {"end", "End"},
    {"pageup", "Page Up"},
    {"pagedown", "Page Down"},
    {"backspace", "Backspace"},
    {"tab", "Tab"},
    {"capslock", "Caps Lock"},
    {"enter", "Enter"},
#ifdef __APPLE__
    {"shift", "Shift"},
    {"rshift", "Shift"},
    {"ctrl", "Command"},
    {"rctrl", "Command"},
    {"alt", "Alt"},
    {"ralt", "Alt"},
    {"lsuper", "Ctrl"},
    {"rsuper", "Ctrl"},
#else
    {"shift", "Left Shift"},
    {"rshift", "Right Shift"},
    {"ctrl", "Left Ctrl"},
    {"rctrl", "Right Ctrl"},
    {"alt", "Left Alt"},
    {"ralt", "Right Alt"},
    {"lsuper", "Left Super"},
    {"rsuper", "Right Super"},
#endif
    {"space", "Spacebar"},
    {"menu", "Menu"},
    {"escape", "Esc"}};

const KeyName &Keycode::getNames(KeyId keyId) {
  if (keyId > KeyId::LAST_VALID)
    return s_unknownKeyName;
  return s_keyNames[(uint)keyId];
}

KeyId Keycode::fromNativeCode(uint nativeKeyCode) {
#ifdef __linux__
  switch (nativeKeyCode) {
  case 19:
    return KeyId::Num0;
  case 10:
    return KeyId::Num1;
  case 11:
    return KeyId::Num2;
  case 12:
    return KeyId::Num3;
  case 13:
    return KeyId::Num4;
  case 14:
    return KeyId::Num5;
  case 15:
    return KeyId::Num6;
  case 16:
    return KeyId::Num7;
  case 17:
    return KeyId::Num8;
  case 18:
    return KeyId::Num9;
  case 90:
    return KeyId::Numpad0;
  case 87:
    return KeyId::Numpad1;
  case 88:
    return KeyId::Numpad2;
  case 89:
    return KeyId::Numpad3;
  case 83:
    return KeyId::Numpad4;
  case 84:
    return KeyId::Numpad5;
  case 85:
    return KeyId::Numpad6;
  case 79:
    return KeyId::Numpad7;
  case 80:
    return KeyId::Numpad8;
  case 81:
    return KeyId::Numpad9;
  case 67:
    return KeyId::F1;
  case 68:
    return KeyId::F2;
  case 69:
    return KeyId::F3;
  case 70:
    return KeyId::F4;
  case 71:
    return KeyId::F5;
  case 72:
    return KeyId::F6;
  case 73:
    return KeyId::F7;
  case 74:
    return KeyId::F8;
  case 75:
    return KeyId::F9;
  case 76:
    return KeyId::F10;
  case 95:
    return KeyId::F11;
  case 96:
    return KeyId::F12;
  case 191:
    return KeyId::F13;
  case 192:
    return KeyId::F14;
  case 193:
    return KeyId::F15;
  case 194:
    return KeyId::F16;
  case 195:
    return KeyId::F17;
  case 196:
    return KeyId::F18;
  case 197:
    return KeyId::F19;
  case 198:
    return KeyId::F20;
  case 199:
    return KeyId::F21;
  case 200:
    return KeyId::F22;
  case 201:
    return KeyId::F23;
  case 202:
    return KeyId::F24;
  case 38:
    return KeyId::A;
  case 56:
    return KeyId::B;
  case 54:
    return KeyId::C;
  case 40:
    return KeyId::D;
  case 26:
    return KeyId::E;
  case 41:
    return KeyId::F;
  case 42:
    return KeyId::G;
  case 43:
    return KeyId::H;
  case 31:
    return KeyId::I;
  case 44:
    return KeyId::J;
  case 45:
    return KeyId::K;
  case 46:
    return KeyId::L;
  case 58:
    return KeyId::M;
  case 57:
    return KeyId::N;
  case 32:
    return KeyId::O;
  case 33:
    return KeyId::P;
  case 24:
    return KeyId::Q;
  case 27:
    return KeyId::R;
  case 39:
    return KeyId::S;
  case 28:
    return KeyId::T;
  case 30:
    return KeyId::U;
  case 55:
    return KeyId::V;
  case 25:
    return KeyId::W;
  case 53:
    return KeyId::X;
  case 29:
    return KeyId::Y;
  case 52:
    return KeyId::Z;
  case 49:
    return KeyId::Grave;
  case 20:
    return KeyId::Hyphen;
  case 21:
    return KeyId::Equals;
  case 34:
    return KeyId::LeftBracket;
  case 35:
    return KeyId::RightBracket;
  case 51:
    return KeyId::Backslash;
  case 47:
    return KeyId::Semicolon;
  case 48:
    return KeyId::Quote;
  case 59:
    return KeyId::Comma;
  case 60:
    return KeyId::Period;
  case 61:
    return KeyId::Slash;
  case 106:
    return KeyId::NumpadSlash;
  case 63:
    return KeyId::NumpadAsterisk;
  case 82:
    return KeyId::NumpadMinus;
  case 86:
    return KeyId::NumpadPlus;
  case 104:
    return KeyId::NumpadEnter;
  case 91:
    return KeyId::NumpadPeriod;
  case 77:
    return KeyId::NumLock;
  case 111:
    return KeyId::ArrowUp;
  case 113:
    return KeyId::ArrowLeft;
  case 116:
    return KeyId::ArrowDown;
  case 114:
    return KeyId::ArrowRight;
  case 107:
    return KeyId::PrintScreen;
  case 78:
    return KeyId::ScrollLock;
  case 127:
    return KeyId::Break;
  case 118:
    return KeyId::Insert;
  case 119:
    return KeyId::Delete;
  case 110:
    return KeyId::Home;
  case 115:
    return KeyId::End;
  case 112:
    return KeyId::PageUp;
  case 117:
    return KeyId::PageDown;
  case 22:
    return KeyId::Backspace;
  case 23:
    return KeyId::Tab;
  case 66:
    return KeyId::CapsLock;
  case 36:
    return KeyId::Enter;
  case 50:
    return KeyId::LeftShift;
  case 62:
    return KeyId::RightShift;
  case 37:
    return KeyId::LeftControl;
  case 105:
    return KeyId::RightControl;
  case 64:
    return KeyId::LeftAlt;
  case 108:
    return KeyId::RightAlt;
  case 133:
    return KeyId::LeftSuper;
  case 134:
    return KeyId::RightSuper;
  case 65:
    return KeyId::Space;
  case 135:
    return KeyId::Menu;
  case 9:
    return KeyId::Escape;
  default:
    return KeyId::INVALID;
  }
#endif

#ifdef _WIN32
  switch (nativeKeyCode) {
  case 11:
    return KeyId::Num0;
  case 2:
    return KeyId::Num1;
  case 3:
    return KeyId::Num2;
  case 4:
    return KeyId::Num3;
  case 5:
    return KeyId::Num4;
  case 6:
    return KeyId::Num5;
  case 7:
    return KeyId::Num6;
  case 8:
    return KeyId::Num7;
  case 9:
    return KeyId::Num8;
  case 10:
    return KeyId::Num9;
  case 82:
    return KeyId::Numpad0;
  case 79:
    return KeyId::Numpad1;
  case 80:
    return KeyId::Numpad2;
  case 81:
    return KeyId::Numpad3;
  case 75:
    return KeyId::Numpad4;
  case 76:
    return KeyId::Numpad5;
  case 77:
    return KeyId::Numpad6;
  case 71:
    return KeyId::Numpad7;
  case 72:
    return KeyId::Numpad8;
  case 73:
    return KeyId::Numpad9;
  case 59:
    return KeyId::F1;
  case 60:
    return KeyId::F2;
  case 61:
    return KeyId::F3;
  case 62:
    return KeyId::F4;
  case 63:
    return KeyId::F5;
  case 64:
    return KeyId::F6;
  case 65:
    return KeyId::F7;
  case 66:
    return KeyId::F8;
  case 67:
    return KeyId::F9;
  case 68:
    return KeyId::F10;
  case 87:
    return KeyId::F11;
  case 88:
    return KeyId::F12;
  case 100:
    return KeyId::F13;
  case 101:
    return KeyId::F14;
  case 102:
    return KeyId::F15;
  case 103:
    return KeyId::F16;
  case 104:
    return KeyId::F17;
  case 105:
    return KeyId::F18;
  case 106:
    return KeyId::F19;
  case 107:
    return KeyId::F20;
  case 108:
    return KeyId::F21;
  case 109:
    return KeyId::F22;
  case 110:
    return KeyId::F23;
  case 111:
    return KeyId::F24;
  case 30:
    return KeyId::A;
  case 48:
    return KeyId::B;
  case 46:
    return KeyId::C;
  case 32:
    return KeyId::D;
  case 18:
    return KeyId::E;
  case 33:
    return KeyId::F;
  case 34:
    return KeyId::G;
  case 35:
    return KeyId::H;
  case 23:
    return KeyId::I;
  case 36:
    return KeyId::J;
  case 37:
    return KeyId::K;
  case 38:
    return KeyId::L;
  case 50:
    return KeyId::M;
  case 49:
    return KeyId::N;
  case 24:
    return KeyId::O;
  case 25:
    return KeyId::P;
  case 16:
    return KeyId::Q;
  case 19:
    return KeyId::R;
  case 31:
    return KeyId::S;
  case 20:
    return KeyId::T;
  case 22:
    return KeyId::U;
  case 47:
    return KeyId::V;
  case 17:
    return KeyId::W;
  case 45:
    return KeyId::X;
  case 21:
    return KeyId::Y;
  case 44:
    return KeyId::Z;
  case 41:
    return KeyId::Grave;
  case 12:
    return KeyId::Hyphen;
  case 13:
    return KeyId::Equals;
  case 26:
    return KeyId::LeftBracket;
  case 27:
    return KeyId::RightBracket;
  case 43:
    return KeyId::Backslash;
  case 39:
    return KeyId::Semicolon;
  case 40:
    return KeyId::Quote;
  case 51:
    return KeyId::Comma;
  case 52:
    return KeyId::Period;
  case 53:
    return KeyId::Slash;
  case 309:
    return KeyId::NumpadSlash;
  case 55:
    return KeyId::NumpadAsterisk;
  case 74:
    return KeyId::NumpadMinus;
  case 78:
    return KeyId::NumpadPlus;
  case 284:
    return KeyId::NumpadEnter;
  case 83:
    return KeyId::NumpadPeriod;
  /* NumLock doesn't seem to get recognized in Qt on Windows */
  case 328:
    return KeyId::ArrowUp;
  case 331:
    return KeyId::ArrowLeft;
  case 336:
    return KeyId::ArrowDown;
  case 333:
    return KeyId::ArrowRight;
  case 84:
    return KeyId::PrintScreen;
  case 70:
    return KeyId::ScrollLock;
  case 69:
    return KeyId::Break;
  case 338:
    return KeyId::Insert;
  case 339:
    return KeyId::Delete;
  case 327:
    return KeyId::Home;
  case 335:
    return KeyId::End;
  case 329:
    return KeyId::PageUp;
  case 337:
    return KeyId::PageDown;
  case 14:
    return KeyId::Backspace;
  case 15:
    return KeyId::Tab;
  case 58:
    return KeyId::CapsLock;
  case 28:
    return KeyId::Enter;
  case 42:
    return KeyId::LeftShift;
  case 54:
    return KeyId::RightShift;
  case 29:
    return KeyId::LeftControl;
  case 285:
    return KeyId::RightControl;
  case 56:
    return KeyId::LeftAlt;
  case 312:
    return KeyId::RightAlt;
  case 347:
    return KeyId::LeftSuper;
  case 348:
    return KeyId::RightSuper;
  case 349:
    return KeyId::Menu;
  case 57:
    return KeyId::Space;
  case 1:
    return KeyId::Escape;
  default:
    return KeyId::INVALID;
  }
#endif

#ifdef __APPLE__
  switch (nativeKeyCode) {
  case 29:
    return KeyId::Num0;
  case 18:
    return KeyId::Num1;
  case 19:
    return KeyId::Num2;
  case 20:
    return KeyId::Num3;
  case 21:
    return KeyId::Num4;
  case 23:
    return KeyId::Num5;
  case 22:
    return KeyId::Num6;
  case 26:
    return KeyId::Num7;
  case 28:
    return KeyId::Num8;
  case 25:
    return KeyId::Num9;
  case 82:
    return KeyId::Numpad0;
  case 83:
    return KeyId::Numpad1;
  case 84:
    return KeyId::Numpad2;
  case 85:
    return KeyId::Numpad3;
  case 86:
    return KeyId::Numpad4;
  case 87:
    return KeyId::Numpad5;
  case 88:
    return KeyId::Numpad6;
  case 89:
    return KeyId::Numpad7;
  case 91:
    return KeyId::Numpad8;
  case 92:
    return KeyId::Numpad9;
  case 122:
    return KeyId::F1;
  case 120:
    return KeyId::F2;
  case 99:
    return KeyId::F3;
  case 118:
    return KeyId::F4;
  case 96:
    return KeyId::F5;
  case 97:
    return KeyId::F6;
  case 98:
    return KeyId::F7;
  case 100:
    return KeyId::F8;
  case 101:
    return KeyId::F9;
  case 109:
    return KeyId::F10;
  case 103:
    return KeyId::F11;
  case 111:
    return KeyId::F12;
  // F13 on Mac is PrntScrn
  case 107:
    return KeyId::F14;
  case 113:
    return KeyId::F15;
  case 106:
    return KeyId::F16;
  case 64:
    return KeyId::F17;
  case 79:
    return KeyId::F18;
  case 80:
    return KeyId::F19;
  case 90:
    return KeyId::F20;
  /* F21 to F24 don't exist on Mac I guess? */
  case 0:
    return KeyId::A;
  case 11:
    return KeyId::B;
  case 8:
    return KeyId::C;
  case 2:
    return KeyId::D;
  case 14:
    return KeyId::E;
  case 3:
    return KeyId::F;
  case 5:
    return KeyId::G;
  case 4:
    return KeyId::H;
  case 34:
    return KeyId::I;
  case 38:
    return KeyId::J;
  case 40:
    return KeyId::K;
  case 37:
    return KeyId::L;
  case 46:
    return KeyId::M;
  case 45:
    return KeyId::N;
  case 31:
    return KeyId::O;
  case 35:
    return KeyId::P;
  case 12:
    return KeyId::Q;
  case 15:
    return KeyId::R;
  case 1:
    return KeyId::S;
  case 17:
    return KeyId::T;
  case 32:
    return KeyId::U;
  case 9:
    return KeyId::V;
  case 13:
    return KeyId::W;
  case 7:
    return KeyId::X;
  case 16:
    return KeyId::Y;
  case 6:
    return KeyId::Z;
  case 50:
    return KeyId::Grave;
  case 27:
    return KeyId::Hyphen;
  case 24:
    return KeyId::Equals;
  case 33:
    return KeyId::LeftBracket;
  case 30:
    return KeyId::RightBracket;
  case 42:
    return KeyId::Backslash;
  case 41:
    return KeyId::Semicolon;
  case 39:
    return KeyId::Quote;
  case 43:
    return KeyId::Comma;
  case 47:
    return KeyId::Period;
  case 44:
    return KeyId::Slash;
  case 75:
    return KeyId::NumpadSlash;
  case 67:
    return KeyId::NumpadAsterisk;
  case 78:
    return KeyId::NumpadMinus;
  case 69:
    return KeyId::NumpadPlus;
  case 76:
    return KeyId::NumpadEnter;
  case 65:
    return KeyId::NumpadPeriod;
  /* Numlock doesn't exist on Mac? Or at least Qt doesn't detect it. */
  case 126:
    return KeyId::ArrowUp;
  case 123:
    return KeyId::ArrowLeft;
  case 125:
    return KeyId::ArrowDown;
  case 124:
    return KeyId::ArrowRight;
  case 105:
    return KeyId::PrintScreen;
  /* Scroll Lock and Pause/Break don't seem to work on Mac? */
  /* Insert doesn't exist either */
  case 117:
    return KeyId::Delete;
  case 115:
    return KeyId::Home;
  case 119:
    return KeyId::End;
  case 116:
    return KeyId::PageUp;
  case 121:
    return KeyId::PageDown;
  case 51:
    return KeyId::Backspace;
  case 48:
    return KeyId::Tab;
  case 57:
    return KeyId::CapsLock;
  case 36:
    return KeyId::Enter;
  case 56:
    return KeyId::LeftShift;
  case 60:
    return KeyId::RightShift;
  case 59:
    return KeyId::LeftControl;
  case 62:
    return KeyId::RightControl;
  case 58:
    return KeyId::LeftAlt;
  case 61:
    return KeyId::RightAlt;
  case 55:
    return KeyId::LeftSuper;
  /* There is no right Super/Windows/Command key on Mac */
  case 49:
    return KeyId::Space;
  case 110:
    return KeyId::Menu;
  case 53:
    return KeyId::Escape;
  default:
    return KeyId::INVALID;
  }
#endif
}

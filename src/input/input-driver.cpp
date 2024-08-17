#include "src/input/input-driver.hpp"

extern InputDriver InputDriverUtil::getDefault() {
#ifdef __APPLE__
  return InputDriver::HID;
#else
  return InputDriver::SDL;
#endif
}

extern const std::set<InputDriver> &InputDriverUtil::getSupported() {
  static const std::set<InputDriver> supportedDrivers = {
#if defined(__linux__)
    InputDriver::SDL,
    InputDriver::JoyDev
#elif defined(__APPLE__)
    InputDriver::HID
#elif defined(_WIN32)
    InputDriver::SDL,
    InputDriver::DirectInput
#endif
  };
  return supportedDrivers;
}

static constexpr char D_HID[] = "hid";
static constexpr char D_SDL[] = "sdl";
static constexpr char D_DINPUT[] = "dinput";
static constexpr char D_JOYDEV[] = "joydev";

template <>
void JsonSerializer::serialize<InputDriver>(JsonWriter &jw,
                                            const InputDriver &obj) {
  const InputDriver driver =
      InputDriverUtil::isSupported(obj) ? obj : InputDriverUtil::getDefault();
  switch (driver) {
  case InputDriver::HID:
    jw.writeString(D_HID);
    break;
  case InputDriver::SDL:
    jw.writeString(D_SDL);
    break;
  case InputDriver::DirectInput:
    jw.writeString(D_DINPUT);
    break;
  case InputDriver::JoyDev:
    jw.writeString(D_JOYDEV);
    break;
  default:
    jw.writeString("null");
    break;
  }
}

template <> InputDriver JsonSerializer::parse<InputDriver>(const Json &json) {
  const string driver = json.getOrDefault<string>("null");

  if (driver == D_HID && InputDriverUtil::isSupported(InputDriver::HID)) {
    return InputDriver::HID;
  }

  if (driver == D_SDL && InputDriverUtil::isSupported(InputDriver::SDL)) {
    return InputDriver::SDL;
  }

  if (driver == D_DINPUT &&
      InputDriverUtil::isSupported(InputDriver::DirectInput)) {
    return InputDriver::DirectInput;
  }

  if (driver == D_JOYDEV && InputDriverUtil::isSupported(InputDriver::JoyDev)) {
    return InputDriver::JoyDev;
  }

  return InputDriverUtil::getDefault();
}

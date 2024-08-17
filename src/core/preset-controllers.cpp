#include "preset-controllers.hpp"

#include <QtGlobal>
#include <unordered_set>

static const HashSet<ControllerId> s_gamecube = {
    {0x057E, 0x0337}, // Official
    {0x0079, 0x1844}  // Mayflash
};

static const HashSet<ControllerId> s_xbox360 = {
    {0x045E, 0x028E}, // Official, wired
    {0x045E, 0x028F}, // Official, wireless
    {0x0E6F, 0x0213}, // Afterglow
    {0x0E6F, 0x0413}, // Afterglow AX.1
    {0x12AB, 0x0301}, // Afterglow (Hong Kong)
    {0x046D, 0x0242}, // Logitech
    {0x0738, 0x4716}, // Mad Catz, wired
    {0x0738, 0x4726}, // Mad Cats, wireless
    {0x0E6F, 0x011F}, // Rock Candy, wired
    {0x0E6F, 0x021F}, // Rock Candy, wireless
    {0x0F0D, 0x000C}, // Hori
    {0x146B, 0x0601}, // BigBen
    {0x15E4, 0x3F0A}, // Numark Airflo
    {0x24C6, 0x5300}, // ThrustMaster (Mini ProEX)
    {0x24C6, 0x5303}, // ThrustMaster (Airflo)
    {0x24C6, 0x530A}, // ThrustMaster (ProEX)
    {0x24C6, 0xFAFD}, // ThrustMaster (Afterglow)
    {0x24C6, 0xFAFE}, // ThrustMaster (Rock Candy)
    {0x28DE, 0x11FF}, // Steam Deck
};

static const HashSet<ControllerId> s_nintendo64 = {
    {0x0079, 0x0002} // Retrolink
};

static const HashSet<ControllerId> s_switchPro = {
    {0x057E, 0x2009} // Official
};

ControllerType getControllerType(const ControllerId &controllerId) {
  if (s_gamecube.count(controllerId) > 0) {
    return ControllerType::Gamecube;
  }

  if (s_xbox360.count(controllerId) > 0) {
    return ControllerType::XBox360;
  }

  if (s_nintendo64.count(controllerId) > 0) {
    return ControllerType::Nintendo64;
  }

  if (s_switchPro.count(controllerId) > 0) {
    return ControllerType::SwitchPro;
  }

  return ControllerType::Other;
}

static const ControllerProfile s_xboxHidProfile = {
    /* Name */ "Default XBox360 Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 5},
     /* CDown */ {BindingType::AxisPositive, 5},
     /* CLeft */ {BindingType::AxisNegative, 4},
     /* CRight */ {BindingType::AxisPositive, 4},
     /* DPadUp */ {BindingType::HatUp, 0},
     /* DPadDown */ {BindingType::HatDown, 0},
     /* DPadLeft */ {BindingType::HatLeft, 0},
     /* DPadRight */ {BindingType::HatRight, 0},
     /* ButtonA */ {BindingType::Button, 0},
     /* ButtonB */ {BindingType::Button, 3},
     /* TriggerL */ {BindingType::Button, 7},
     /* TriggerZ */ {BindingType::AxisPositive, 10},
     /* TriggerR */ {BindingType::AxisPositive, 9},
     /* Start */ {BindingType::Button, 11},
     /* ButtonX */ {BindingType::Button, 1},
     /* ButtonY */ {BindingType::Button, 2},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

static const ControllerProfile s_gamecubeSdlProfile = {
    /* Name */ "Default Gamecube Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 3},
     /* CDown */ {BindingType::AxisPositive, 3},
     /* CLeft */ {BindingType::AxisNegative, 2},
     /* CRight */ {BindingType::AxisPositive, 2},
     /* DPadUp */ {BindingType::Button, 11},
     /* DPadDown */ {BindingType::Button, 12},
     /* DPadLeft */ {BindingType::Button, 13},
     /* DPadRight */ {BindingType::Button, 14},
     /* ButtonA */ {BindingType::Button, 0},
#ifdef _WIN32
     /* ButtonB */ {BindingType::Button, 1},
#else
      /* ButtonB */ {BindingType::Button, 3},
#endif
     /* TriggerL */ {BindingType::Button, 10},
     /* TriggerZ */ {BindingType::AxisPositive, 4},
     /* TriggerR */ {BindingType::AxisPositive, 5},
     /* Start */ {BindingType::Button, 6},
#ifdef _WIN32
     /* ButtonX */ {BindingType::Button, 3},
#else
      /* ButtonX */ {BindingType::Button, 1},
#endif
     /* ButtonY */ {BindingType::Button, 2},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.15,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

static const ControllerProfile s_xboxSdlProfile = {
    /* Name */ "Default XBox360 Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 3},
     /* CDown */ {BindingType::AxisPositive, 3},
     /* CLeft */ {BindingType::AxisNegative, 2},
     /* CRight */ {BindingType::AxisPositive, 2},
     /* DPadUp */ {BindingType::Button, 11},
     /* DPadDown */ {BindingType::Button, 12},
     /* DPadLeft */ {BindingType::Button, 13},
     /* DPadRight */ {BindingType::Button, 14},
     /* ButtonA */ {BindingType::Button, 0},
     /* ButtonB */ {BindingType::Button, 2},
     /* TriggerL */ {BindingType::Button, 10},
     /* TriggerZ */ {BindingType::AxisPositive, 4},
     /* TriggerR */ {BindingType::AxisPositive, 5},
     /* Start */ {BindingType::Button, 6},
     /* ButtonX */ {BindingType::Button, 1},
     /* ButtonY */ {BindingType::Button, 3},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

const ControllerProfile s_n64SdlProfile = {
    /* Name */ "Default USB N64 Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::Button, 0},
     /* CDown */ {BindingType::Button, 2},
     /* CLeft */ {BindingType::Button, 3},
     /* CRight */ {BindingType::Button, 1},
     /* DPadUp */ {BindingType::HatUp, 0},
     /* DPadDown */ {BindingType::HatDown, 0},
     /* DPadLeft */ {BindingType::HatLeft, 0},
     /* DPadRight */ {BindingType::HatRight, 0},
     /* ButtonA */ {BindingType::Button, 5},
     /* ButtonB */ {BindingType::Button, 4},
     /* TriggerL */ {BindingType::Button, 6},
     /* TriggerZ */ {BindingType::Button, 8},
     /* TriggerR */ {BindingType::Button, 7},
     /* Start */ {BindingType::Button, 9},
     /* ButtonX */ {BindingType::None, 0},
     /* ButtonY */ {BindingType::None, 0},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

const ControllerProfile s_switchSdlProfile = {
    /* Name */ "Default Switch Pro Controller Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 3},
     /* CDown */ {BindingType::AxisPositive, 3},
     /* CLeft */ {BindingType::AxisNegative, 2},
     /* CRight */ {BindingType::AxisPositive, 2},
     /* DPadUp */ {BindingType::Button, 11},
     /* DPadDown */ {BindingType::Button, 12},
     /* DPadLeft */ {BindingType::Button, 13},
     /* DPadRight */ {BindingType::Button, 14},
     /* ButtonA */ {BindingType::Button, 0},
#ifdef _WIN32
     /* ButtonB */ {BindingType::Button, 2},
#else
      /* ButtonB */ {BindingType::Button, 3},
#endif
     /* TriggerL */ {BindingType::Button, 10},
     /* TriggerZ */ {BindingType::AxisPositive, 4},
     /* TriggerR */ {BindingType::AxisPositive, 5},
     /* Start */ {BindingType::Button, 6},
#ifdef _WIN32
     /* ButtonX */ {BindingType::Button, 3},
#else
      /* ButtonX */ {BindingType::Button, 2},
#endif
     /* ButtonY */ {BindingType::Button, 1},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

static const ControllerProfile s_xboxDirectInputProfile = {
    /* Name */ "Default XBox360 Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 4},
     /* CDown */ {BindingType::AxisPositive, 4},
     /* CLeft */ {BindingType::AxisNegative, 3},
     /* CRight */ {BindingType::AxisPositive, 3},
     /* DPadUp */ {BindingType::HatUp, 0},
     /* DPadDown */ {BindingType::HatDown, 0},
     /* DPadLeft */ {BindingType::HatLeft, 0},
     /* DPadRight */ {BindingType::HatRight, 0},
     /* ButtonA */ {BindingType::Button, 0},
     /* ButtonB */ {BindingType::Button, 2},
     /* TriggerL */ {BindingType::Button, 5},
     /* TriggerZ */ {BindingType::AxisPositive, 2},
     /* TriggerR */ {BindingType::AxisNegative, 2},
     /* Start */ {BindingType::Button, 7},
     /* ButtonX */ {BindingType::Button, 1},
     /* ButtonY */ {BindingType::Button, 3},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

static const ControllerProfile s_gamecubeDirectInputProfile = {
    /* Name */ "Default Gamecube Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 2},
     /* CDown */ {BindingType::AxisPositive, 2},
     /* CLeft */ {BindingType::AxisNegative, 5},
     /* CRight */ {BindingType::AxisPositive, 5},
     /* DPadUp */ {BindingType::HatUp, 0},
     /* DPadDown */ {BindingType::HatDown, 0},
     /* DPadLeft */ {BindingType::HatLeft, 0},
     /* DPadRight */ {BindingType::HatRight, 0},
     /* ButtonA */ {BindingType::Button, 1},
     /* ButtonB */ {BindingType::Button, 2},
     /* TriggerL */ {BindingType::Button, 7},
     /* TriggerZ */ {BindingType::AxisPositive, 3},
     /* TriggerR */ {BindingType::AxisPositive, 4},
     /* Start */ {BindingType::Button, 9},
     /* ButtonX */ {BindingType::Button, 0},
     /* ButtonY */ {BindingType::Button, 3},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.15,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

static const ControllerProfile s_xboxJoyDevProfile = {
    /* Name */ "Default XBox360 Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 4},
     /* CDown */ {BindingType::AxisPositive, 4},
     /* CLeft */ {BindingType::AxisNegative, 3},
     /* CRight */ {BindingType::AxisPositive, 3},
     /* DPadUp */ {BindingType::AxisNegative, 7},
     /* DPadDown */ {BindingType::AxisPositive, 7},
     /* DPadLeft */ {BindingType::AxisNegative, 6},
     /* DPadRight */ {BindingType::AxisPositive, 6},
     /* ButtonA */ {BindingType::Button, 0},
     /* ButtonB */ {BindingType::Button, 2},
     /* TriggerL */ {BindingType::Button, 5},
     /* TriggerZ */ {BindingType::AxisPositive, 2},
     /* TriggerR */ {BindingType::AxisPositive, 5},
     /* Start */ {BindingType::Button, 7},
     /* ButtonX */ {BindingType::Button, 1},
     /* ButtonY */ {BindingType::Button, 3},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

static const ControllerProfile s_gamecubeJoyDevProfile = {
    /* Name */ "Default Gamecube Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 4},
     /* CDown */ {BindingType::AxisPositive, 4},
     /* CLeft */ {BindingType::AxisNegative, 3},
     /* CRight */ {BindingType::AxisPositive, 3},
     /* DPadUp */ {BindingType::Button, 8},
     /* DPadDown */ {BindingType::Button, 9},
     /* DPadLeft */ {BindingType::Button, 10},
     /* DPadRight */ {BindingType::Button, 11},
     /* ButtonA */ {BindingType::Button, 0},
     /* ButtonB */ {BindingType::Button, 3},
     /* TriggerL */ {BindingType::Button, 6},
     /* TriggerZ */ {BindingType::AxisPositive, 2},
     /* TriggerR */ {BindingType::AxisPositive, 5},
     /* Start */ {BindingType::Button, 7},
     /* ButtonX */ {BindingType::Button, 1},
     /* ButtonY */ {BindingType::Button, 2},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

static const ControllerProfile s_switchJoyDevProfile = {
    /* Name */ "Default Switch Pro Controller Profile",
    /* Bindings */
    {/* AnalogUp */ {BindingType::AxisNegative, 1},
     /* AnalogDown */ {BindingType::AxisPositive, 1},
     /* AnalogLeft */ {BindingType::AxisNegative, 0},
     /* AnalogRight */ {BindingType::AxisPositive, 0},
     /* CUp */ {BindingType::AxisNegative, 3},
     /* CDown */ {BindingType::AxisPositive, 3},
     /* CLeft */ {BindingType::AxisNegative, 2},
     /* CRight */ {BindingType::AxisPositive, 2},
     /* DPadUp */ {BindingType::AxisNegative, 5},
     /* DPadDown */ {BindingType::AxisPositive, 5},
     /* DPadLeft */ {BindingType::AxisNegative, 4},
     /* DPadRight */ {BindingType::AxisPositive, 4},
     /* ButtonA */ {BindingType::Button, 0},
     /* ButtonB */ {BindingType::Button, 3},
     /* TriggerL */ {BindingType::Button, 6},
     /* TriggerZ */ {BindingType::Button, 7},
     /* TriggerR */ {BindingType::Button, 8},
     /* Start */ {BindingType::Button, 11},
     /* ButtonX */ {BindingType::Button, 2},
     /* ButtonY */ {BindingType::Button, 1},
     /* SaveState */ {BindingType::None, 0},
     /* LoadState */ {BindingType::None, 0},
     /* FastForward */ {BindingType::None, 0},
     /* SlowMotion */ {BindingType::None, 0}},
    /* Sensitivity */ 1.0,
    /* Deadzone */ 0.15,
    /* Button Axis Threshold */ 0.70,
    /* Rumble */ false};

bool DefaultProfile::exists(InputDriver driver, const string &name) {
  switch (driver) {
  case InputDriver::HID:
    return name == s_xboxHidProfile.name;
  case InputDriver::SDL:
    return (name == s_xboxSdlProfile.name ||
            name == s_gamecubeSdlProfile.name || name == s_n64SdlProfile.name ||
            name == s_switchSdlProfile.name);
  case InputDriver::DirectInput:
    return (name == s_xboxDirectInputProfile.name ||
            name == s_gamecubeDirectInputProfile.name);
  case InputDriver::JoyDev:
    return (name == s_xboxJoyDevProfile.name ||
            name == s_gamecubeJoyDevProfile.name ||
            name == s_switchJoyDevProfile.name);
  default:
    return false;
  }
}

const ControllerProfile &DefaultProfile::get(InputDriver driver,
                                             ControllerType type) {
  switch (driver) {
  case InputDriver::HID: {
    return s_xboxHidProfile;
  }
  case InputDriver::SDL: {
    switch (type) {
    case ControllerType::Gamecube:
      return s_gamecubeSdlProfile;
    case ControllerType::Nintendo64:
      return s_n64SdlProfile;
    case ControllerType::SwitchPro:
      return s_switchSdlProfile;
    default:
      return s_xboxSdlProfile;
    }
  }
  case InputDriver::DirectInput: {
    return (type == ControllerType::Gamecube) ? s_gamecubeDirectInputProfile
                                              : s_xboxDirectInputProfile;
  }
  case InputDriver::JoyDev: {
    switch (type) {
    case ControllerType::Gamecube:
      return s_gamecubeJoyDevProfile;
    case ControllerType::SwitchPro:
      return s_switchJoyDevProfile;
    default:
      return s_xboxJoyDevProfile;
    }
  }
  default: {
    return s_xboxSdlProfile;
  }
  }
}

const std::vector<ControllerProfile> &
DefaultProfile::getAll(InputDriver driver) {
  switch (driver) {
  case InputDriver::HID: {
    static const std::vector<ControllerProfile> s_profiles = {s_xboxHidProfile};
    return s_profiles;
  }
  case InputDriver::SDL: {
    static const std::vector<ControllerProfile> s_profiles = {
        s_xboxSdlProfile, s_gamecubeSdlProfile, s_n64SdlProfile,
        s_switchSdlProfile};
    return s_profiles;
  }
  case InputDriver::DirectInput: {
    static const std::vector<ControllerProfile> s_profiles = {
        s_xboxDirectInputProfile, s_gamecubeDirectInputProfile};
    return s_profiles;
  }
  case InputDriver::JoyDev: {
    static const std::vector<ControllerProfile> s_profiles = {
        s_xboxJoyDevProfile, s_gamecubeJoyDevProfile, s_switchJoyDevProfile};
    return s_profiles;
  }
  default: {
    static const std::vector<ControllerProfile> s_profiles;
    return s_profiles;
  }
  }
}

const InputMode DefaultInputModes::Normal = {
    Uuid::parse("00000000-0000-4000-8000-000000000000"),
    QT_TRANSLATE_NOOP("InputModeSelect", "Normal"),
    QT_TRANSLATE_NOOP("InputModeSelect",
                      "Maps your gamepad inputs to a single N64 controller "
                      "using your controller profile"),
    {ControllerAction::AnalogUp,   ControllerAction::AnalogDown,
     ControllerAction::AnalogLeft, ControllerAction::AnalogRight,
     ControllerAction::CUp,        ControllerAction::CDown,
     ControllerAction::CLeft,      ControllerAction::CRight,
     ControllerAction::DPadUp,     ControllerAction::DPadDown,
     ControllerAction::DPadLeft,   ControllerAction::DPadRight,
     ControllerAction::ButtonA,    ControllerAction::ButtonB,
     ControllerAction::TriggerL,   ControllerAction::TriggerZ,
     ControllerAction::TriggerR,   ControllerAction::Start,
     ControllerAction::ButtonX,    ControllerAction::ButtonY},
    {ControllerAction::None, ControllerAction::None, ControllerAction::None,
     ControllerAction::None, ControllerAction::None, ControllerAction::None,
     ControllerAction::None, ControllerAction::None, ControllerAction::None,
     ControllerAction::None, ControllerAction::None, ControllerAction::None,
     ControllerAction::None, ControllerAction::None, ControllerAction::None,
     ControllerAction::None, ControllerAction::None, ControllerAction::None,
     ControllerAction::None, ControllerAction::None}};

const InputMode DefaultInputModes::DualAnalog = {
    Uuid::parse("00000000-0000-4000-8000-000000000001"),
    QT_TRANSLATE_NOOP("InputModeSelect", "Dual Analog"),
    QT_TRANSLATE_NOOP(
        "InputModeSelect",
        "Your gamepad inputs that normally bind to the C buttons instead bind "
        "to the analog stick on a second N64 controller"),
    {ControllerAction::AnalogUp,   ControllerAction::AnalogDown,
     ControllerAction::AnalogLeft, ControllerAction::AnalogRight,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::DPadUp,     ControllerAction::DPadDown,
     ControllerAction::DPadLeft,   ControllerAction::DPadRight,
     ControllerAction::ButtonA,    ControllerAction::ButtonB,
     ControllerAction::TriggerL,   ControllerAction::TriggerZ,
     ControllerAction::TriggerR,   ControllerAction::Start,
     ControllerAction::ButtonX,    ControllerAction::ButtonY},
    {ControllerAction::CUp,    ControllerAction::CDown, ControllerAction::CLeft,
     ControllerAction::CRight, ControllerAction::None,  ControllerAction::None,
     ControllerAction::None,   ControllerAction::None,  ControllerAction::None,
     ControllerAction::None,   ControllerAction::None,  ControllerAction::None,
     ControllerAction::None,   ControllerAction::None,  ControllerAction::None,
     ControllerAction::None,   ControllerAction::None,  ControllerAction::None,
     ControllerAction::None,   ControllerAction::None}};

const InputMode DefaultInputModes::GoldenEye = {
    Uuid::parse("00000000-0000-4000-8000-000000000002"),
    QT_TRANSLATE_NOOP("InputModeSelect", "GoldenEye"),
    QT_TRANSLATE_NOOP(
        "InputModeSelect",
        "Maps your gamepad inputs to two N64 controllers suitable for playing "
        "GoldenEye with the 2.4 Goodhead control style"),
    {ControllerAction::CUp,      ControllerAction::CDown,
     ControllerAction::CLeft,    ControllerAction::CRight,
     ControllerAction::None,     ControllerAction::None,
     ControllerAction::None,     ControllerAction::None,
     ControllerAction::DPadUp,   ControllerAction::DPadDown,
     ControllerAction::DPadLeft, ControllerAction::DPadRight,
     ControllerAction::ButtonA,  ControllerAction::ButtonB,
     ControllerAction::TriggerL, ControllerAction::TriggerZ,
     ControllerAction::None,     ControllerAction::Start,
     ControllerAction::ButtonX,  ControllerAction::ButtonY},
    {ControllerAction::AnalogUp,   ControllerAction::AnalogDown,
     ControllerAction::AnalogLeft, ControllerAction::AnalogRight,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::None,       ControllerAction::TriggerR,
     ControllerAction::None,       ControllerAction::None,
     ControllerAction::None,       ControllerAction::None}};

const InputMode DefaultInputModes::Clone = {
    Uuid::parse("00000000-0000-4000-8000-000000000003"),
    QT_TRANSLATE_NOOP("InputModeSelect", "Clone"),
    QT_TRANSLATE_NOOP("InputModeSelect",
                      "Your gamepad inputs are sent to two controller ports "
                      "instead of just one"),
    {ControllerAction::AnalogUp,   ControllerAction::AnalogDown,
     ControllerAction::AnalogLeft, ControllerAction::AnalogRight,
     ControllerAction::CUp,        ControllerAction::CDown,
     ControllerAction::CLeft,      ControllerAction::CRight,
     ControllerAction::DPadUp,     ControllerAction::DPadDown,
     ControllerAction::DPadLeft,   ControllerAction::DPadRight,
     ControllerAction::ButtonA,    ControllerAction::ButtonB,
     ControllerAction::TriggerL,   ControllerAction::TriggerZ,
     ControllerAction::TriggerR,   ControllerAction::Start,
     ControllerAction::ButtonX,    ControllerAction::ButtonY},
    {ControllerAction::AnalogUp,   ControllerAction::AnalogDown,
     ControllerAction::AnalogLeft, ControllerAction::AnalogRight,
     ControllerAction::CUp,        ControllerAction::CDown,
     ControllerAction::CLeft,      ControllerAction::CRight,
     ControllerAction::DPadUp,     ControllerAction::DPadDown,
     ControllerAction::DPadLeft,   ControllerAction::DPadRight,
     ControllerAction::ButtonA,    ControllerAction::ButtonB,
     ControllerAction::TriggerL,   ControllerAction::TriggerZ,
     ControllerAction::TriggerR,   ControllerAction::Start,
     ControllerAction::ButtonX,    ControllerAction::ButtonY}};

bool DefaultInputModes::exists(const Uuid &uuid) {
  return uuid == DefaultInputModes::Normal.id ||
         uuid == DefaultInputModes::DualAnalog.id ||
         uuid == DefaultInputModes::GoldenEye.id ||
         uuid == DefaultInputModes::Clone.id;
}

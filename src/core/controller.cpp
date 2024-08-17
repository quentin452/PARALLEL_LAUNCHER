#include "src/core/controller.hpp"
#include "src/core/numeric-string.hpp"
#include <cstring>

template <>
void JsonSerializer::serialize<Binding>(JsonWriter &jw, const Binding &obj) {
  switch (obj.type) {
  case BindingType::None:
    jw.writeNull();
    break;
  case BindingType::Button:
    jw.writeString(Number::toString(obj.buttonOrAxis));
    break;
  case BindingType::AxisNegative:
    jw.writeString("-"s + Number::toString(obj.buttonOrAxis));
    break;
  case BindingType::AxisPositive:
    jw.writeString("+"s + Number::toString(obj.buttonOrAxis));
    break;
  case BindingType::HatUp:
    jw.writeString("h^"s + Number::toString(obj.buttonOrAxis));
    break;
  case BindingType::HatDown:
    jw.writeString("hv"s + Number::toString(obj.buttonOrAxis));
    break;
  case BindingType::HatLeft:
    jw.writeString("h<"s + Number::toString(obj.buttonOrAxis));
    break;
  case BindingType::HatRight:
    jw.writeString("h>"s + Number::toString(obj.buttonOrAxis));
    break;
  }
}

template <> Binding JsonSerializer::parse<Binding>(const Json &json) {
  if (json.isNull())
    return {BindingType::None, 0};
  const string inputString = json.get<string>();

  switch (inputString.at(0)) {
  case '+':
    return {BindingType::AxisPositive,
            (ushort)Number::parseUInt(inputString.substr(1))};
  case '-':
    return {BindingType::AxisNegative, (ushort)-Number::parseInt(inputString)};
  case 'h': {
    const ushort index = (ushort)Number::parseUInt(inputString.substr(2));
    switch (inputString.at(1)) {
    case '^':
      return {BindingType::HatUp, index};
    case 'v':
      return {BindingType::HatDown, index};
    case '<':
      return {BindingType::HatLeft, index};
    case '>':
      return {BindingType::HatRight, index};
    default:
      break;
    }
    [[fallthrough]];
  }
  default:
    return {BindingType::Button, (ushort)Number::parseUInt(inputString)};
  }
}

static constexpr char P_NAME[] = "name";
static constexpr char P_DESCRIPTION[] = "description";
static constexpr char P_UUID[] = "uuid";
static constexpr char P_BINDINGS[] = "bindings";
static constexpr char P_SENSITIVITY[] = "sensitivity";
static constexpr char P_DEADZONE[] = "deadzone";
static constexpr char P_AXIS_THRESHOLD[] = "axisThreshold";
static constexpr char P_RUMBLE[] = "rumble";
static constexpr char P_MAP1[] = "map1";
static constexpr char P_MAP2[] = "map2";

template <>
void JsonSerializer::serialize<ControllerProfile>(
    JsonWriter &jw, const ControllerProfile &obj) {
  jw.writeObjectStart();
  jw.writeProperty(P_NAME, obj.name);
  jw.writePropertyName(P_BINDINGS);
  jw.writeArrayStart();
  for (int i = 0; i < (int)ControllerAction::NUM_ACTIONS; i++) {
    JsonSerializer::serialize<Binding>(jw, obj.bindings[i]);
  }
  jw.writeArrayEnd();
  jw.writeProperty(P_SENSITIVITY, obj.sensitivity);
  jw.writeProperty(P_DEADZONE, obj.deadzone);
  jw.writeProperty(P_AXIS_THRESHOLD, obj.axisThreshold);
  jw.writeProperty(P_RUMBLE, obj.rumble);
  jw.writeObjectEnd();
}

static void insertXYButtonBindingsIfMissing(JArray &bindingsJson) {
  // Check if this json is for an old version before the X and Y buttons were
  // added
  bool migratingFromOldJson = bindingsJson.size() == 22u;
  // If it is, insert two dummy bindings where the X and Y buttons were added
  if (migratingFromOldJson) {
    // X and Y were inserted at position 18, so insert two null json values at
    // that index
    bindingsJson.emplace(bindingsJson.begin() + 18u, JsonInternals::JNull());
    bindingsJson.emplace(bindingsJson.begin() + 18u, JsonInternals::JNull());
  }
}

template <>
ControllerProfile JsonSerializer::parse<ControllerProfile>(const Json &json) {
  ControllerProfile profile;
  profile.name = json[P_NAME].get<string>();
  JArray bindingsJson = json[P_BINDINGS].array();

  insertXYButtonBindingsIfMissing(bindingsJson);

  for (size_t i = 0; i < (size_t)ControllerAction::NUM_ACTIONS; i++) {
    if (bindingsJson.size() > i) {
      profile.bindings[i] = JsonSerializer::parse<Binding>(bindingsJson[i]);
    } else {
      profile.bindings[i] = {BindingType::None, 0};
    }
  }
  profile.sensitivity = json[P_SENSITIVITY].get<double>();
  profile.deadzone = json[P_DEADZONE].get<double>();
  profile.axisThreshold = json[P_AXIS_THRESHOLD].getOrDefault<double>(0.70);
  profile.rumble = json[P_RUMBLE].get<bool>();
  return profile;
}

template <>
void JsonSerializer::serialize<InputMode>(JsonWriter &jw,
                                          const InputMode &obj) {
  jw.writeObjectStart();
  jw.writeProperty(P_UUID, obj.id);
  jw.writeProperty(P_NAME, obj.name);
  jw.writeProperty(P_DESCRIPTION, obj.description);
  jw.writePropertyName(P_MAP1);
  jw.writeArrayStart();
  for (size_t i = 0; i < (size_t)ControllerAction::NUM_INPUTS; i++) {
    jw.writeNumber((uint64)obj.port1[i]);
  }
  jw.writeArrayEnd();
  jw.writePropertyName(P_MAP2);
  jw.writeArrayStart();
  for (size_t i = 0; i < (size_t)ControllerAction::NUM_INPUTS; i++) {
    jw.writeNumber((uint64)obj.port2[i]);
  }
  jw.writeArrayEnd();
  jw.writeObjectEnd();
}

template <> InputMode JsonSerializer::parse<InputMode>(const Json &json) {
  InputMode inputMode;
  inputMode.id = Uuid::parse(json[P_UUID].get<string>());
  inputMode.name = json[P_NAME].get<string>();
  inputMode.description = json[P_DESCRIPTION].get<string>();

  const JArray &map1 = json[P_MAP1].array();
  const JArray &map2 = json[P_MAP2].array();
  for (size_t i = 0; i < (size_t)ControllerAction::NUM_INPUTS; i++) {
    inputMode.port1[i] = map1[i].get<ControllerAction>();
    inputMode.port2[i] = map2[i].get<ControllerAction>();
  }

  return inputMode;
}

static const InputMapping s_unbound = {
    ControllerAction::None, ControllerAction::None, ControllerAction::None,
    ControllerAction::None, ControllerAction::None, ControllerAction::None,
    ControllerAction::None, ControllerAction::None, ControllerAction::None,
    ControllerAction::None, ControllerAction::None, ControllerAction::None,
    ControllerAction::None, ControllerAction::None, ControllerAction::None,
    ControllerAction::None, ControllerAction::None, ControllerAction::None,
    ControllerAction::None, ControllerAction::None};

static_assert(s_unbound.size() == (size_t)ControllerAction::NUM_INPUTS);

bool InputMode::usesTwoPorts() const {
  return std::memcmp((void *)port2.data(), (void *)s_unbound.data(),
                     sizeof(InputMapping)) != 0;
}

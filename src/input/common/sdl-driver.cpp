#include "src/input/common/sdl-driver.hpp"

#include "src/core/logging.hpp"
#include "src/core/qthread.hpp"
#include "src/input/sdl-lsjs.hpp"
#include "src/polyfill/base-directory.hpp"
#if defined(__linux__) && !defined(FLATPAK_VERSION)
#include <SDL2/SDL.h>
#include <dlfcn.h>
#endif
#ifdef _WIN32
#include "src/core/file-controller.hpp"
#include "src/polyfill/env.hpp"
#endif

static constexpr ubyte SPE_CONNECT = 0;
static constexpr ubyte SPE_DISCONNECT = 1;
static constexpr ubyte SPE_BUTTON = 2;
static constexpr ubyte SPE_AXIS = 3;
static constexpr ubyte SPE_HAT = 4;

template <typename T>
static inline bool tryRead(ReadableFifoPipe &pipe, T &dest) {
  std::error_code err;
  return pipe.read(sizeof(T), &dest, err);
}

template <> inline bool tryRead<string>(ReadableFifoPipe &pipe, string &dest) {
  dest.clear();

  char c;
  std::error_code err;
  while (pipe.read(1, &c, err)) {
    if (c == '\0')
      return true;
    dest += c;
  }

  return false;
}

template <> inline bool tryRead<Uuid>(ReadableFifoPipe &pipe, Uuid &dest) {
  std::error_code err;
  ubyte data[16];
  if (!pipe.read(16, data, err))
    return false;
  dest = Uuid(data);
  return true;
}

SdlDriver::SdlDriver(const GamepadEventEmitter &eventEmitter)
    : IGamepadDriver(eventEmitter) {
  m_running = std::make_shared<bool>(false);
  m_sessionNumber = 0;
  m_sync = false;
}

SdlDriver::~SdlDriver() { stop(); }

#ifdef _WIN32
#define EXE_EXTENSION L".exe"
#else
#define EXE_EXTENSION
#endif

void SdlDriver::start() {
  if (*m_running)
    return;

#ifdef _WIN32
  Environment::set("SDL_JOYSTICK_RAWINPUT",
                   FileController::loadAppSettings().sdlRawInput ? "1" : "0");
#endif

  m_sessionNumber++;
  *m_running = true;

  std::error_code err;
  m_pipe = ReadableFifoPipe();
  if (!m_pipe.open(Uuid::Random().toString(), err)) {
    logError("Failed to create sdl-relay pipe: "s + err.message());
    *m_running = false;
    return;
  }

  std::vector<string> args;
  args.push_back(m_pipe.filePath());
  if (m_sync)
    args.push_back("--sync");

  m_gamepads.clear();
  try {
    m_proxy = AsyncProcess(
        (BaseDir::share() / (_NFS("parallel-launcher-sdl-relay") EXE_EXTENSION))
            .u8string(),
        args);
  } catch (...) {
    *m_running = false;
    m_pipe.close();
    throw;
  }

  if (!m_pipe.connect(err)) {
    logError("Failed to create sdl-relay pipe: "s + err.message());
    *m_running = false;
    m_pipe.close();
    return;
  }

  m_runner =
      std::thread(SdlDriver::asyncRunner, this, m_running, m_sessionNumber);
}

void SdlDriver::stop() {
  if (!*m_running)
    return;
  *m_running = false;
  m_pipe.close();
  m_proxy.kill();
  m_proxy.join();
  if (m_runner.joinable())
    m_runner.join();
  m_proxy = AsyncProcess();
}

bool SdlDriver::isRunning() const { return *m_running; }

std::vector<ConnectedGamepad> SdlDriver::getConnected() {
  if (!*m_running) {
    m_sync = true;
    start();
    m_proxy.join();
    if (m_runner.joinable())
      m_runner.join();
    m_sync = false;
    stop();
  }

  std::vector<ConnectedGamepad> gamepads;
  gamepads.reserve(m_gamepads.size());
  for (const auto &gamepad : m_gamepads) {
    gamepads.push_back({gamepad.first, gamepad.second.info});
  }
  return gamepads;
}

std::vector<Uuid> SdlDriver::getDeviceOrder() {
  bool success = false;
  std::vector<Uuid> uuids = lsjs::tryGetPortOrder(success);
  if (!success) {
    logError("Failed to run lsjs. Controllers will not be bound correctly.");
  }
  return uuids;
}

GamepadState SdlDriver::getState(GamepadId id) const {
  const auto gamepad = m_gamepads.find(id);
  if (gamepad == m_gamepads.end())
    return GamepadState{};
  return gamepad->second.state;
}

void SdlDriver::asyncRunner(SdlDriver *self, std::shared_ptr<bool> running,
                            uint sessionNumber) {
  std::error_code err;
  while (*running && sessionNumber == self->m_sessionNumber) {
    ubyte eventType;
    if (!self->m_pipe.read(1, &eventType, err))
      break;
    if (!handleEvent(self, running, sessionNumber, eventType))
      break;
  }

  QtThread::safeAsync([=]() {
    if (*running && self->m_sessionNumber == sessionNumber) {
      self->stop();
    }
  });
}

bool SdlDriver::handleEvent(SdlDriver *self, std::shared_ptr<bool> running,
                            uint sessionNumber, ubyte eventType) {
  std::error_code err;
  switch (eventType) {
  case SPE_CONNECT: {
    GamepadConnectedEvent event;
    GamepadState state;

    if (!tryRead(self->m_pipe, event.id) ||
        !tryRead(self->m_pipe, event.info.name) ||
        !tryRead(self->m_pipe, event.info.controllerId.vendorId) ||
        !tryRead(self->m_pipe, event.info.controllerId.productId) ||
        !tryRead(self->m_pipe, event.info.uuid) ||
        !tryRead(self->m_pipe, event.info.numButtons) ||
        !tryRead(self->m_pipe, event.info.numAxes) ||
        !tryRead(self->m_pipe, event.info.numHats)) {
      return false;
    }

    state.buttons.reserve(event.info.numButtons);
    for (ubyte i = 0; i < event.info.numButtons; i++) {
      ubyte buttonState;
      if (!tryRead(self->m_pipe, buttonState))
        return false;
      state.buttons.push_back(buttonState > 0);
    }

    state.axes.reserve(event.info.numAxes);
    for (ubyte i = 0; i < event.info.numAxes; i++) {
      short axisState;
      if (!tryRead(self->m_pipe, axisState))
        return false;
      state.axes.push_back((double)axisState /
                           (axisState >= 0 ? (double)0x7FFF : (double)0x8000));
    }

    state.hats.reserve(event.info.numHats);
    for (ubyte i = 0; i < event.info.numHats; i++) {
      ubyte hatState;
      if (!tryRead(self->m_pipe, hatState))
        return false;
      state.hats.push_back((HatDirection)hatState);
    }

    if (self->m_sync) {
      self->m_gamepads[event.id] = SdlGamepad{event.info, state};
      self->m_events.gamepadConnected(event);
      self->m_gamepads[event.id].info = event.info;
    } else {
      QtThread::safeAsync([=]() {
        if (!*running || self->m_sessionNumber != sessionNumber)
          return;
        GamepadConnectedEvent mutableEvent = event;
        self->m_gamepads[event.id] = SdlGamepad{event.info, state};
        self->m_events.gamepadConnected(mutableEvent);
        self->m_gamepads[event.id].info = mutableEvent.info;
      });
    }
    return true;
  }
  case SPE_DISCONNECT: {
    GamepadDisconnectedEvent event;
    if (!tryRead(self->m_pipe, event.id))
      return false;
    QtThread::safeAsync([=]() {
      if (!*running || self->m_sessionNumber != sessionNumber)
        return;
      self->m_gamepads.erase(event.id);
      self->m_events.gamepadDisconnected(event);
    });
    return true;
  }
  case SPE_BUTTON: {
    GamepadButtonEvent event;
    ubyte state;
    if (!tryRead(self->m_pipe, event.id) ||
        !tryRead(self->m_pipe, event.button) || !tryRead(self->m_pipe, state))
      return false;
    event.isPressed = (state > 0);
    QtThread::safeAsync([=]() {
      if (!*running || self->m_sessionNumber != sessionNumber)
        return;
      if (!self->m_gamepads.count(event.id))
        return;
      self->m_events.gamepadButtonPressed(event);
      self->m_gamepads[event.id].state.buttons[event.button] = event.isPressed;
    });
    return true;
  }
  case SPE_AXIS: {
    GamepadAxisEvent event;
    short state;
    if (!tryRead(self->m_pipe, event.id) ||
        !tryRead(self->m_pipe, event.axis) || !tryRead(self->m_pipe, state))
      return false;
    event.position =
        (double)state / (state >= 0 ? (double)0x7FFF : (double)0x8000);
    QtThread::safeAsync([=]() {
      if (!*running || self->m_sessionNumber != sessionNumber)
        return;
      if (!self->m_gamepads.count(event.id))
        return;
      self->m_events.gamepadAxisMoved(event);
      self->m_gamepads[event.id].state.axes[event.axis] = event.position;
    });
    return true;
  }
  case SPE_HAT: {
    GamepadHatEvent event;
    ubyte state;
    if (!tryRead(self->m_pipe, event.id) || !tryRead(self->m_pipe, event.hat) ||
        !tryRead(self->m_pipe, state))
      return false;
    event.position = (HatDirection)state;
    QtThread::safeAsync([=]() {
      if (!*running || self->m_sessionNumber != sessionNumber)
        return;
      if (!self->m_gamepads.count(event.id))
        return;
      self->m_events.gamepadHatMoved(event);
      self->m_gamepads[event.id].state.hats[event.hat] = event.position;
    });
    return true;
  }
  default: {
    logError("Unexpected event type received from sdl-relay");
    return false;
  }
  }
}

#if defined(__linux__) && !defined(FLATPAK_VERSION)
std::string SdlDriver::getSdlLibraryPath() noexcept {
  Dl_info dlInfo;
  if (!dladdr((const void *)SDL_JoystickOpen, &dlInfo))
    return std::string();
  if (!dlInfo.dli_fname)
    return std::string();
  return std::string(dlInfo.dli_fname);
}

Version SdlDriver::getSdlVersion() {
  SDL_version version;
  SDL_GetVersion(&version);
  return Version{version.major, version.minor, version.patch};
}
#endif

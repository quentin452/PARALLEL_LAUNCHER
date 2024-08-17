#ifndef SRC_INPUT_COMMON_SDL_DRIVER_HPP_
#define SRC_INPUT_COMMON_SDL_DRIVER_HPP_

#include "src/core/version.hpp"
#include "src/input/gamepad-input-driver.hpp"
#include "src/polyfill/fifo-pipe.hpp"
#include "src/polyfill/process.hpp"
#include <map>
#include <memory>
#include <thread>

struct SdlGamepad {
  ControllerInfo info;
  GamepadState state;
};

class SdlDriver : public IGamepadDriver {

private:
  std::shared_ptr<bool> m_running;
  std::map<GamepadId, SdlGamepad> m_gamepads;
  std::thread m_runner;
  AsyncProcess m_proxy;
  ReadableFifoPipe m_pipe;
  uint m_sessionNumber;
  bool m_sync;

public:
  SdlDriver(const GamepadEventEmitter &eventEmitter);
  ~SdlDriver();

  InputDriver id() const override { return InputDriver::SDL; }

  void start() override;
  void stop() override;
  bool isRunning() const override;

  std::vector<ConnectedGamepad> getConnected() override;
  std::vector<Uuid> getDeviceOrder() override;
  GamepadState getState(GamepadId id) const override;

#if defined(__linux__) && !defined(FLATPAK_VERSION)
  static std::string getSdlLibraryPath() noexcept;
  static Version getSdlVersion();
#endif

private:
  static void asyncRunner(SdlDriver *self, std::shared_ptr<bool> running,
                          uint sessionNumber);
  static bool handleEvent(SdlDriver *self, std::shared_ptr<bool> running,
                          uint sessionNumber, ubyte eventType);
};

#endif /* SRC_INPUT_COMMON_SDL_DRIVER_HPP_ */

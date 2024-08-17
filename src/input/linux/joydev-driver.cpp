#if defined(__linux__)
#include "src/input/linux/joydev-driver.hpp"

#include "src/core/buffer.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/numeric-string.hpp"
#include "src/polyfill/file.hpp"
#include <QTimer>
#include <chrono>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <map>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

static inline int tryOpenNewFile(const string &filename) {
  // TODO: should do this in a better way that doesn't block
  for (int i = 0; i < 10; i++) {
    const int fd = open(filename.c_str(), O_RDONLY);
    if (fd >= 0)
      return fd;
    if (errno != EACCES)
      return -1;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return -1;
}

class PosixFile {

private:
  string m_path;
  int m_fd;

public:
  PosixFile() : m_path(""), m_fd(-1) {}

  PosixFile(const string &filePath)
      : m_path(filePath), m_fd(tryOpenNewFile(filePath)) {}

  PosixFile(string &&filePath)
      : m_path(std::move(filePath)), m_fd(tryOpenNewFile(m_path)) {}

  ~PosixFile() {
    if (m_fd >= 0)
      close(m_fd);
  }

  PosixFile(const PosixFile &other) = delete;
  PosixFile(PosixFile &&other)
      : m_path(std::move(other.m_path)), m_fd(other.m_fd) {
    other.m_fd = -1;
  }

  PosixFile &operator=(const PosixFile &other) = delete;
  PosixFile &operator=(PosixFile &&other) {
    m_path = std::move(other.m_path);
    m_fd = other.m_fd;
    other.m_fd = -1;
    return *this;
  }

  inline const string &path() const noexcept { return m_path; }
  inline int fd() const noexcept { return m_fd; }
  inline bool isValid() const noexcept { return m_fd >= 0; }
};

struct JoydevGamepad {
  ControllerInfo info;
  GamepadState state;
  GamepadId gamepadId;
  PosixFile file;
};

static GamepadId s_nextId = 1;
static HashMap<GamepadId, JoydevGamepad> s_devices;
static int s_inotify = -1;
static int s_watcher = -1;
static QTimer s_timer;

static inline string tryGetEventFile(const string &jsPath) {
  const string target = fs::path(jsPath).filename().string();

  InputFile devFile("/proc/bus/input/devices", true);

  string line;
  while (std::getline(devFile, line).good() && !devFile.eof()) {
    if (line.substr(0, 12) != "H: Handlers=")
      continue;
    line = line.substr(12);

    bool match = false;
    string eventFile = "";
    while (!line.empty()) {
      const size_t si = line.find_first_of(' ');
      const string part = line.substr(0, si);

      if (part == target) {
        match = true;
      } else if (part.size() >= 6 && part.substr(0, 5) == "event") {
        eventFile = part;
      }

      line = line.substr(si + 1);
    }

    if (match && !eventFile.empty()) {
      return "/dev/input/"s + eventFile;
    }
  }

  return "";
}

static bool tryOpenGamepad(const string &jsPath, JoydevGamepad &gamepad) {
  PosixFile jsFile(jsPath);
  if (!jsFile.isValid())
    return false;

  const string eventPath = tryGetEventFile(jsPath);
  if (eventPath.empty())
    return false;

  PosixFile eventFile(eventPath);
  if (!eventFile.isValid())
    return false;

  input_id jid;
  if (ioctl(eventFile.fd(), EVIOCGID, &jid) < 0) {
    return false;
  }

  ubyte uuidData[16] __attribute__((aligned(4)));
  ((ushort *)uuidData)[0] = jid.bustype;
  ((ushort *)uuidData)[1] = jid.product;
  ((ushort *)uuidData)[2] = jid.vendor;
  ((ushort *)uuidData)[3] = jid.version;

  if (ioctl(eventFile.fd(), EVIOCGPROP(sizeof(uint)), &uuidData[8]) < 0) {
    ((uint *)uuidData)[2] = 0;
  }

  char charBuffer[256];
  string name = "";
  if (ioctl(jsFile.fd(), JSIOCGNAME(256), charBuffer) >= 0) {
    charBuffer[255] = '\0';
    name = charBuffer;

    if (name.size() > 4) {
      uuidData[12] = (ubyte)name[0];
      uuidData[13] = (ubyte)name[1];
      uuidData[14] = (ubyte)name[name.size() - 2];
      uuidData[15] = (ubyte)name[name.size() - 1];
    } else
      for (size_t i = 0; i < name.size(); i++) {
        uuidData[12 + i] = (ubyte)name[i];
      }
  } else {
    name = "Unknown ("s + jsPath + ')';
  }

  ubyte numButtons;
  if (ioctl(jsFile.fd(), JSIOCGBUTTONS, &numButtons) < 0) {
    numButtons = 255;
  }

  ubyte numAxes;
  if (ioctl(jsFile.fd(), JSIOCGAXES, &numAxes) < 0) {
    numAxes = 80u;
  }

  gamepad.info.name = std::move(name);
  gamepad.info.controllerId = ControllerId{jid.vendor, jid.product};
  gamepad.info.uuid = Uuid(uuidData);
  gamepad.info.numButtons = numButtons;
  gamepad.info.numAxes = numAxes;
  gamepad.info.numHats = 0;
  gamepad.state = {std::vector<bool>(numButtons, false),
                   std::vector<double>(numAxes, 0.0),
                   std::vector<HatDirection>(0)};
  gamepad.gamepadId = s_nextId++;
  gamepad.file = std::move(jsFile);

  return true;
}

JoyDevDriver::JoyDevDriver(const GamepadEventEmitter &eventEmitter)
    : IGamepadDriver(eventEmitter) {
  QObject::connect(&s_timer, &QTimer::timeout,
                   [this]() { this->processEvents(); });
  s_timer.setInterval(10);
}

JoyDevDriver::~JoyDevDriver() { stop(); }

void JoyDevDriver::start() {
  stop();

  s_inotify = inotify_init();
  s_watcher = inotify_add_watch(s_inotify, "/dev/input", IN_CREATE | IN_DELETE);

  loadConnectedDevices(true);

  s_timer.start();
}

void JoyDevDriver::stop() {
  if (s_inotify < 0)
    return;

  s_timer.stop();

  if (s_watcher >= 0) {
    inotify_rm_watch(s_inotify, s_watcher);
    s_watcher = -1;
  }

  close(s_inotify);
  s_inotify = -1;

  s_devices.clear();
}

bool JoyDevDriver::isRunning() const { return s_inotify >= 0; }

static inline double normalize(short axisPosn) {
  return (double)axisPosn / (axisPosn >= 0 ? (double)0x7FFF : (double)0x8000);
}

static inline bool tryFindGamepad(const string &file, GamepadId *gamepadId) {
  foreach_cvalue(gamepad, s_devices) {
    if (gamepad.file.path() == file) {
      if (gamepadId != nullptr) {
        *gamepadId = gamepad.gamepadId;
      }
      return true;
    }
  }

  return false;
}

void JoyDevDriver::loadConnectedDevices(bool sendEvents) const {
  fs::error_code err;
  for (fs::directory_iterator folder(fs::path("/dev/input"), err);
       !err && folder != fs::end(folder); folder.increment(err)) {
    const fs::directory_entry &file = *folder;

    if (file.is_directory())
      continue;
    if (file.path().filename().string().substr(0, 2) != "js")
      continue;

    JoydevGamepad gamepad;
    if (!tryOpenGamepad(file.path().string(), gamepad))
      continue;

    const GamepadId gamepadId = gamepad.gamepadId;
    s_devices[gamepadId] = std::move(gamepad);

    if (sendEvents) {
      GamepadConnectedEvent event = {gamepadId, s_devices[gamepadId].info};
      m_events.gamepadConnected(event);
    }
  }
}

struct BasicINotifyEvent {
  uint32_t type;
  string name;
};

static inline bool tryReadSingleEvent(BasicINotifyEvent &event) {
  for (size_t size = 2 * sizeof(inotify_event) - 1; size < 8192;
       size += sizeof(inotify_event)) {
    Buffer buffer(size);
    const size_t eventSize = read(s_inotify, buffer.data(), size);

    if (eventSize == (size_t)-1) {
      if (errno == EINVAL)
        continue;
      return false;
    }

    if (eventSize < sizeof(inotify_event))
      return false;

    const inotify_event &rawEvent = *((inotify_event *)buffer.data());
    event.type = rawEvent.mask;
    event.name = string(rawEvent.name);
    return true;
  }
  return false;
}

void JoyDevDriver::processEvents() const {
  if (s_inotify < 0)
    return;

  /* poll inotify */ {
    pollfd fds[] = {{s_inotify, POLLIN, 0}};
    while (poll(fds, 1, 0) && (fds[0].revents & POLLIN)) {
      BasicINotifyEvent event;

      if (!tryReadSingleEvent(event))
        continue;
      if (event.name.substr(0, 2) != "js")
        continue;

      const string filePath = "/dev/input/"s + event.name;
      if (event.type == IN_CREATE) {
        if (tryFindGamepad(filePath, nullptr))
          continue;
        JoydevGamepad gamepad;
        if (tryOpenGamepad(filePath, gamepad)) {
          const GamepadId gamepadId = gamepad.gamepadId;
          s_devices[gamepadId] = std::move(gamepad);
          GamepadConnectedEvent event = {gamepadId, s_devices[gamepadId].info};
          m_events.gamepadConnected(event);
        }
      } else if (event.type == IN_DELETE) {
        GamepadId gamepadId = 0;
        if (!tryFindGamepad(filePath, &gamepadId))
          continue;
        m_events.gamepadDisconnected({gamepadId});
        s_devices.erase(gamepadId);
      }
    }
  }

  std::vector<GamepadId> pendingRemovals;
  foreach_mvalue(device, s_devices) {
    pollfd fds[] = {{device.file.fd(), POLLIN, 0}};
    while (poll(fds, 1, 0) && (fds[0].revents & POLLIN)) {
      js_event event;
      if (read(device.file.fd(), (void *)&event, sizeof(js_event)) !=
          sizeof(js_event)) {
        m_events.gamepadDisconnected({device.gamepadId});
        pendingRemovals.push_back(device.gamepadId);
      }

      switch (event.type & 0x7F) {
      case JS_EVENT_BUTTON:
        if ((ushort)event.number >= device.info.numButtons)
          continue;
        device.state.buttons[event.number] = (event.value > 0);
        m_events.gamepadButtonPressed(
            {device.gamepadId, event.number, event.value > 0});
        break;
      case JS_EVENT_AXIS: {
        if ((ushort)event.number >= device.info.numAxes)
          continue;
        const double posn = normalize(event.value);
        device.state.axes[event.number] = posn;
        m_events.gamepadAxisMoved({device.gamepadId, event.number, posn});
        break;
      }
      default:
        continue;
      }
    }
  }

  for (GamepadId gamepadId : pendingRemovals) {
    s_devices.erase(gamepadId);
  }
}

std::vector<ConnectedGamepad> JoyDevDriver::getConnected() {
  if (!isRunning()) {
    loadConnectedDevices(false);
  }

  std::vector<ConnectedGamepad> gamepads;
  gamepads.reserve(s_devices.size());
  foreach_cvalue(device, s_devices) {
    gamepads.push_back({device.gamepadId, device.info});
  }

  if (!isRunning()) {
    s_devices.clear();
  }

  return gamepads;
}

std::vector<Uuid> JoyDevDriver::getDeviceOrder() {
  if (!isRunning()) {
    loadConnectedDevices(false);
  }

  std::map<int, Uuid> deviceOrder;
  foreach_cvalue(device, s_devices) {
    const int port = Number::parseInt(device.file.path().substr(13));
    deviceOrder[port] = device.info.uuid;
  }

  if (!isRunning()) {
    s_devices.clear();
  }

  std::vector<Uuid> result;
  result.reserve(deviceOrder.size());
  foreach_cvalue(uuid, deviceOrder) { result.push_back(uuid); }

  return result;
}

GamepadState JoyDevDriver::getState(GamepadId id) const {
  const auto &i = s_devices.find(id);
  if (i == s_devices.end())
    return GamepadState{};
  return i->second.state;
}
#endif // defined(__linux__)
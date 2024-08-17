#include "src/rhdc/core/libpl-rhdc.hpp"

#include "src/core/qthread.hpp"
#include "src/core/time.hpp"
#include "src/rhdc/core/credentials.hpp"
#include "src/rhdc/web/api-helpers.hpp"
#include "src/rhdc/web/api.hpp"
#include <QImage>
#include <mutex>
#include <thread>
#include <unordered_map>

constexpr ubyte LPL_OKAY = 0;
constexpr ubyte LPL_COMMAND_NOT_FOUND = 1;
constexpr ubyte LPL_INVALID_PAYLOAD_SIZE = 2;
constexpr ubyte LPL_BROKEN_PIPE = 3;
constexpr ubyte LPL_BAD_STATE = 4;
constexpr ubyte LPL_WAIT = 5;
constexpr ubyte LPL_INVALID_PAYLOAD = 6;

enum class AsyncStatus : ubyte {
  NotStarted = 0,
  Pending = 1,
  Ready = 2,
  Error = 3,
  Retry = 4
};

struct CachedAvatar {
  ubyte rgba16[2048];
  ubyte rgba32[4096];
  AsyncStatus status;
  ubyte errorCode;

  inline CachedAvatar() : status(AsyncStatus::NotStarted) {}
};

static std::mutex s_lock;
static int s_sessionId = 0;

static string s_username;
static bool s_usernameFetched;
static HashMap<string, CachedAvatar> s_avatars;

static inline bool writeHeader(const WritableFifoPipe &pipe, ubyte status1,
                               ubyte status2, ushort payloadSize,
                               std::error_code &err) {
  const ubyte header[4] = {status1, status2, (ubyte)(payloadSize >> 8),
                           (ubyte)(payloadSize & 0xff)};
  return pipe.write(4, header, err);
}

void clearRhdcLibplCache() {
  const std::lock_guard lock(s_lock);
  s_sessionId++;
  s_username.clear();
  s_avatars.clear();
  s_usernameFetched = false;
}

static bool handleGetMyUsernameRequest(const WritableFifoPipe &pipe,
                                       std::error_code &err) {
  const std::lock_guard lock(s_lock);

  if (!s_usernameFetched) {
    s_username = ApiUtil::identity().username;
    if (s_username.empty()) {
      s_username = RhdcCredentials::load().username;
    }
    s_usernameFetched = true;
  }

  if (s_username.empty()) {
    return writeHeader(pipe, LPL_OKAY, 1, 0, err);
  }

  if (!writeHeader(pipe, LPL_OKAY, 0, (ushort)s_username.length() + 1, err))
    return false;
  return pipe.write(s_username.length() + 1, s_username.c_str(), err);
}

static inline void setAvatar(CachedAvatar &avatar, const QByteArray &imgData) {
  if (avatar.status != AsyncStatus::Pending)
    return;

  QImage img;
  if (!img.loadFromData(imgData)) {
    avatar.status = AsyncStatus::Error;
    avatar.errorCode = 3;
    return;
  }

  if (img.width() != 32 || img.height() != 32) {
    img = img.scaled(32, 32, Qt::KeepAspectRatioByExpanding,
                     Qt::SmoothTransformation);
  }

#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
  img.convertTo(QImage::Format_ARGB32);
#else
  img = img.convertToFormat(QImage::Format_ARGB32);
#endif
  if (img.sizeInBytes() != 4096) {
    avatar.status = AsyncStatus::Error;
    avatar.errorCode = 3;
    return;
  }

  // This is word-aligned and stored using the native endianness, so the uint*
  // cast is safe
  const uint *pixels = (const uint *)img.constBits();
  ubyte *p16 = avatar.rgba16;
  ubyte *p32 = avatar.rgba32;

  for (int i = 0; i < 1024; i++) {
    const uint pixel = pixels[i];

    const uint r = (pixel >> 16) & 0xFFu;
    const uint g = (pixel >> 8) & 0xFFu;
    const uint b = pixel & 0xFFu;
    const uint a = pixel >> 24;

    *(p32++) = (ubyte)r;
    *(p32++) = (ubyte)g;
    *(p32++) = (ubyte)b;
    *(p32++) = (ubyte)a;

    const ushort sp =
        (((r >> 3) << 11) | ((g >> 3) << 6) | ((b >> 3) << 1) | (a >> 7));

    *(p16++) = (ubyte)(sp >> 8);
    *(p16++) = (ubyte)(sp & 0xFFu);
  }

  avatar.status = AsyncStatus::Ready;
}

static inline void fetchAvatarAsync(const string username) {
  const int sessionId = s_sessionId;
  QtThread::safeAsync([=]() {
    RhdcApi::getAvatarAsync(
        username,
        [=](const QByteArray &imgData) {
          const std::lock_guard lock(s_lock);
          if (sessionId != s_sessionId)
            return;
          setAvatar(s_avatars[username], imgData);
        },
        [=](ApiErrorType error) {
          const std::lock_guard lock(s_lock);
          if (sessionId != s_sessionId)
            return;
          if (error == ApiErrorType::NotFound) {
            s_avatars[username].status = AsyncStatus::Error;
            s_avatars[username].errorCode = 1;
          } else {
            s_avatars[username].status = AsyncStatus::Retry;
            s_avatars[username].errorCode = 2;
          }
        });
  });
}

static bool handleGetAvatarRequest(const WritableFifoPipe &pipe,
                                   ushort payloadSize, const ubyte *payload,
                                   bool rgba32, std::error_code &err) {
  string username;
  if (payloadSize == 0) {
    if (!s_usernameFetched) {
      s_username = ApiUtil::identity().username;
      if (s_username.empty()) {
        s_username = RhdcCredentials::load().username;
      }
      s_usernameFetched = true;
    }

    username = s_username;
  } else if (payloadSize > 30) {
    return writeHeader(pipe, LPL_INVALID_PAYLOAD_SIZE, 1, 0, err);
  } else {
    username = string((const char *)payload, payloadSize);
    for (char c : username) {
      if (c == '-' || c == '_')
        continue;
      if (c >= 'a' && c <= 'z')
        continue;
      if (c >= 'A' && c <= 'Z')
        continue;
      if (c >= '0' && c <= '9')
        continue;
      return writeHeader(pipe, LPL_INVALID_PAYLOAD, 1, 0, err);
    }
  }

  const std::lock_guard lock(s_lock);
  CachedAvatar &avatar = s_avatars[username];
  switch (s_avatars[username].status) {
  case AsyncStatus::NotStarted:
    avatar.status = AsyncStatus::Pending;
    fetchAvatarAsync(username);
    [[fallthrough]];
  case AsyncStatus::Pending:
    return writeHeader(pipe, LPL_WAIT, 0, 0, err);
  case AsyncStatus::Ready:
    if (!writeHeader(pipe, LPL_OKAY, 0, rgba32 ? 4096 : 2048, err))
      return false;
    return pipe.write(rgba32 ? 4096 : 2048,
                      rgba32 ? avatar.rgba32 : avatar.rgba16, err);
  case AsyncStatus::Retry:
    avatar.status = AsyncStatus::NotStarted;
    [[fallthrough]];
  default:
    return writeHeader(pipe, LPL_OKAY, avatar.errorCode, 0, err);
  }
}

bool handleRhdcLibplRequest(ubyte commandId, ushort payloadSize,
                            const ubyte *payload, const WritableFifoPipe &pipe,
                            std::error_code &err) {
  switch (commandId) {
  case 0x00:
    return handleGetMyUsernameRequest(pipe, err);
  case 0x01:
    return handleGetAvatarRequest(pipe, payloadSize, payload, false, err);
  case 0x02:
    return handleGetAvatarRequest(pipe, payloadSize, payload, true, err);
  default:
    return writeHeader(pipe, LPL_COMMAND_NOT_FOUND, 0, 0, err);
  }
}

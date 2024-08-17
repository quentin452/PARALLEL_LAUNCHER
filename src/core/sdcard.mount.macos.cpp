#ifdef __APPLE__

#include "src/core/sdcard.hpp"

#include "src/core/logging.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/process.hpp"
#include <cctype>

static inline string quoteAndEscape(const string &path) {
  string escaped = "\"";
  escaped.reserve(path.size() + 2);
  for (char c : path) {
    if (c == '"' || c == '\\')
      escaped += '\\';
    escaped += c;
  }
  return escaped + '"';
}

static bool tryUnmount(const string &mountPath, bool force) {
  string cmd = "hdiutil unmount "s + quoteAndEscape(mountPath);
  if (force)
    cmd += " -f";

  string errorMessage;
  if (!Process::tryGetErrorOutput(cmd, errorMessage)) {
    logWarn("[hdiutil] "s + errorMessage);
    return false;
  }

  return true;
}

bool MountedSdCard::unmount() {
  if (!m_mounted)
    return true;
  return tryUnmount(m_path, false);
}

MountedSdCard::~MountedSdCard() {
  if (m_mounted)
    tryUnmount(m_path, true);
}

std::shared_ptr<MountedSdCard> SdCardManager::mount(const string &name) {
  const fs::path imagePath = BaseDir::data() / "sdcard" / (name + ".iso");

  string output;
  if (!Process::tryGetOutput("hdiutil mount -readwrite "s +
                                 quoteAndEscape(imagePath.u8string()) + " 2>&1",
                             output)) {
    logWarn("[hdiutil] "s + output);
    return nullptr;
  }

  string path;
  string device;

  size_t i = 0;
  while (i < output.size() && std::isspace(output[i++]))
    ;
  while (i < output.size() && !std::isspace(output[i++]))
    device += output[i - 1];
  while (i < output.size() && std::isspace(output[i++]))
    ;
  while (i < output.size() && !std::isspace(output[i++]))
    path += output[i - 1];

  if (path.empty() || device.empty()) {
    logWarn("Unexpected output from hdiutil: "s + output);
  }

  return std::make_shared<MountedSdCard>(path, device);
}

#endif

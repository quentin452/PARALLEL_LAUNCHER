#ifndef SRC_CORE_CORES_HPP_
#define SRC_CORE_CORES_HPP_

#include "src/core/json.hpp"
#include "src/core/settings.hpp"
#include "src/core/version.hpp"
#include <functional>

struct MupenCoreVersion {
  string commit;
  bool lock;
};

struct RetroArchVersion {
  Version version;
  bool lock;
};

struct CommitInfo {
  string hash;
  string message;
  string date;
};

struct CoreBuild {
  CommitInfo commit;
  string downloadLink;
};

struct InstalledVersionsInfo {
  RetroArchVersion retroArchVersion;
  Version parallelVersion;
  MupenCoreVersion mupenVersion;
  int64 lastUpdateCheck;

  static const InstalledVersionsInfo Default;
};

struct ParallelCoreVersion {
  Version version;
  string sha1;
  string downloadUrl;
};

namespace JsonSerializer {
template <>
void serialize<MupenCoreVersion>(JsonWriter &jw, const MupenCoreVersion &obj);
template <> MupenCoreVersion parse<MupenCoreVersion>(const Json &json);

template <>
void serialize<RetroArchVersion>(JsonWriter &jw, const RetroArchVersion &obj);
template <> RetroArchVersion parse<RetroArchVersion>(const Json &json);

template <>
void serialize<InstalledVersionsInfo>(JsonWriter &jw,
                                      const InstalledVersionsInfo &obj);
template <>
InstalledVersionsInfo parse<InstalledVersionsInfo>(const Json &json);

template <> ParallelCoreVersion parse<ParallelCoreVersion>(const Json &json);
} // namespace JsonSerializer

namespace ParallelCoreBuilds {

void getLatest(
    const std::function<void(const ParallelCoreVersion &)> &onSuccess,
    const std::function<void(void)> &onFailure);

}

#ifndef __APPLE__
namespace MupenCoreBuilds {

void getLastKnownGood(const string &branch, const string &laterThan,
                      const std::function<void(const CoreBuild &)> &onSuccess,
                      const std::function<void(void)> &onFailure);

}
#endif

#ifndef FLATPAK_VERSION
namespace RetroUpdater {

extern void checkForUpdates(bool waitForCoreUpdates, bool forceUpdate);

}
#endif

#endif /* SRC_CORE_CORES_HPP_ */

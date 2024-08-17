#ifndef SRC_RHDC_WEB_API_HPP_
#define SRC_RHDC_WEB_API_HPP_

#include "src/rhdc/core/credentials.hpp"
#include "src/rhdc/core/hack.hpp"
#include "src/rhdc/core/plugin-detect.hpp"
#include "src/rhdc/core/rom-plugin-updater.hpp"
#include "src/types.hpp"
#include <QFile>
#include <functional>
#include <unordered_map>
#include <vector>

enum class HttpStatusCode : int {
  None = 0,

  OK = 200,
  Created = 201,
  Accepted = 202,
  NoContent = 204,

  BadRequest = 400,
  Unauthorized = 401,
  Forbidden = 403,
  NotFound = 404,
  Gone = 410,
  TooManyRequests = 429,

  InternalServerError = 500,
  BadGateway = 502,
  ServiceUnavailable = 503,
  GatewayTimeout = 504
};

enum class ApiErrorType {
  NotAuthorized,
  MfaCodeRequired,
  NotFound,
  ServerDown,
  JsonError,
  UnknownError,
  Offline
};

struct Starpower {
  RhdcHackProgress progress;
  int64 playTime;
};

struct FollowedHack {
  RhdcHackInfo info;
  Starpower starpower;
  std::vector<RhdcHackVersionExt> versions;
  std::vector<string> groups;
  std::vector<string> authors;
  string layoutUrl;
  int64 layoutLastModified;
};

struct RhdcVersionedHack {
  RhdcHackInfo info;
  std::vector<RhdcHackVersionExt> versions;
  std::vector<string> authors;
  string layoutUrl;
  int64 layoutLastModified;
};

struct PluginAndLayoutInfo {
  std::optional<GfxPlugin> plugin;
  PluginFlags pluginFlags;
  string layoutUrl;
};

namespace RhdcApi {

/* login.cpp */

extern bool isAuthenticated() noexcept;

extern void
verifyLoginAsync(const std::function<void(void)> &onSuccess,
                 const std::function<void(ApiErrorType)> &onFailure);

extern void loginAsync(const string &username, const string &password,
                       int mfaCode, bool awaitRetryAfter,
                       const std::function<void(void)> &onSuccess,
                       const std::function<void(ApiErrorType)> &onFailure);

extern void logout() noexcept;

/* games.cpp */

extern void getFollowedHacksAsync(
    const std::function<void(HashMap<string, FollowedHack> &)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure);

extern void
getHackAsync(const string &hackSlug,
             const std::function<void(RhdcVersionedHack &)> &onSuccess,
             const std::function<void(ApiErrorType)> &onFailure);

extern void getLayoutAndRecommendedPluginAsync(
    const string &hackId, const string &versionName,
    const std::function<void(PluginAndLayoutInfo &)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure);

extern void
addHackToListAsync(const string &hackId, const string &group,
                   const std::function<void(void)> &onSuccess,
                   const std::function<void(ApiErrorType)> &onFailure);

extern void
removeHackFromListAsync(const string &hackId, const string &group,
                        const std::function<void(void)> &onSuccess,
                        const std::function<void(ApiErrorType)> &onFailure);

extern void deleteListAsync(const string &group,
                            const std::function<void(void)> &onSuccess,
                            const std::function<void(ApiErrorType)> &onFailure);

extern void downloadHack(const string &downloadUrl, QFile *destinationFile,
                         const std::function<void(int64, int64)> &onProgress,
                         const std::function<void(void)> &onSuccess,
                         const std::function<void(ApiErrorType)> &onFailure);

extern void
downloadThumbnail(const string &hackId, QFile *destinationFile,
                  const std::function<void(void)> &onSuccess,
                  const std::function<void(ApiErrorType)> &onFailure);

extern void getPluginSettings(
    const std::vector<string> &checksums,
    const std::function<void(const std::vector<RhdcSettingHintsExt> &)>
        &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure, int timeoutMs);

extern void getPluginSettings(
    const string &sha1,
    const std::function<void(const std::optional<RhdcSettingHintsExt> &)>
        &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure, int timeoutMs);

/* starpower.cpp */

extern void
getStarpowerAsync(const string &hackId,
                  const std::function<void(Starpower &)> &onSuccess,
                  const std::function<void(ApiErrorType)> &onFailure);

extern void
submitRatingsAsync(const string &hackId, RhdcRating qualityRating,
                   ubyte difficultyRating,
                   const std::function<void(void)> &onSuccess,
                   const std::function<void(ApiErrorType)> &onFailure);

extern void
submitStarProgressAsync(const string &hackId, ushort starsCollected,
                        const std::function<void(void)> &onSuccess,
                        const std::function<void(ApiErrorType)> &onFailure);

extern void
submitCompletionAsync(const string &hackId, bool isComplete,
                      const std::function<void(void)> &onSuccess,
                      const std::function<void(ApiErrorType)> &onFailure);

extern void
submitPlayTimeAsync(const string &hackId, int64 playTime,
                    const std::function<void(void)> &onSuccess,
                    const std::function<void(ApiErrorType)> &onFailure);

/* Telemetry */

extern void sendTelemetryDatum();

extern void sendCrashReports();

/* Misc */

extern void
getAvatarAsync(const string &username,
               const std::function<void(const QByteArray &)> &onSuccess,
               const std::function<void(ApiErrorType)> &onFailure);

extern void downloadFile(const string &downloadUrl, QFile *destinationFile,
                         const std::function<void(int64, int64)> &onProgress,
                         const std::function<void(void)> &onSuccess,
                         const std::function<void(ApiErrorType)> &onFailure);

/* Utility */

extern std::function<void(ApiErrorType)> logApiError(const string &message);

extern string getDownloadUrl(const string &slug, const string &versionName);

}; // namespace RhdcApi

template <>
void JsonSerializer::serialize<RhdcRating>(JsonWriter &jw,
                                           const RhdcRating &quality);
template <> RhdcRating JsonSerializer::parse(const Json &json);

#endif /* SRC_RHDC_WEB_API_HPP_ */

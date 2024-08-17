#include "src/core/updates.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/logging.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/retroarch-sdl-fix.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/time.hpp"
#include "src/core/zip.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/ui/core-installer.hpp"
#include "src/ui/download-dialog.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QMetaObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <queue>
#include <sstream>
#ifdef __APPLE__
#include "src/core/qthread.hpp"
#include "src/polyfill/macos/apple-util.hpp"
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#define ON_NETWORK_ERROR                                                       \
  QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred)
#else
#define ON_NETWORK_ERROR                                                       \
  QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error)
#endif

const InstalledVersionsInfo InstalledVersionsInfo::Default{
    /* retroArchVersion */ RetroArchVersion{Version{0, 0, 0}, false},
    /* parallelVersion */ Version{0, 0, 0},
    /* mupenVersion */ MupenCoreVersion{"", false},
    /* lastUpdateCheck */ 0};

static constexpr char P_COMMIT[] = "commit";
static constexpr char P_LOCK[] = "lock";

template <>
void JsonSerializer::serialize<MupenCoreVersion>(JsonWriter &jw,
                                                 const MupenCoreVersion &obj) {
  jw.writeObjectStart();
  jw.writeProperty(P_COMMIT, obj.commit);
  jw.writeProperty(P_LOCK, obj.lock);
  jw.writeObjectEnd();
}

template <>
MupenCoreVersion JsonSerializer::parse<MupenCoreVersion>(const Json &json) {
  return MupenCoreVersion{json[P_COMMIT].getOrDefault<string>(""),
                          json[P_LOCK].getOrDefault<bool>(false)};
}

static constexpr char P_VERSION[] = "version";

template <>
void JsonSerializer::serialize<RetroArchVersion>(JsonWriter &jw,
                                                 const RetroArchVersion &obj) {
  jw.writeObjectStart();
  jw.writePropertyName(P_VERSION);
  serialize(jw, obj.version);
  jw.writeProperty(P_LOCK, obj.lock);
  jw.writeObjectEnd();
}

template <>
RetroArchVersion JsonSerializer::parse<RetroArchVersion>(const Json &json) {
  Version version = Version{0, 0, 0};
  if (json[P_VERSION].exists()) {
    version = parse<Version>(json[P_VERSION]);
  }

  return RetroArchVersion{version, json[P_LOCK].getOrDefault<bool>(false)};
}

static constexpr char P_RETROARCH[] = "retroarch";
static constexpr char P_PARALLEL[] = "parallel";
static constexpr char P_MUPEN[] = "mupen";
static constexpr char P_LAST_CHECKED[] = "last_checked";

template <>
void JsonSerializer::serialize<InstalledVersionsInfo>(
    JsonWriter &jw, const InstalledVersionsInfo &obj) {
  jw.writeObjectStart();
  jw.writePropertyName(P_RETROARCH);
  serialize(jw, obj.retroArchVersion);
  jw.writePropertyName(P_PARALLEL);
  serialize(jw, obj.parallelVersion);
  jw.writePropertyName(P_MUPEN);
  serialize(jw, obj.mupenVersion);
  jw.writeProperty(P_LAST_CHECKED, obj.lastUpdateCheck);
  jw.writeObjectEnd();
}

template <>
InstalledVersionsInfo
JsonSerializer::parse<InstalledVersionsInfo>(const Json &json) {
  Version parallelVersion;
  try {
    parallelVersion = parse<Version>(json[P_PARALLEL]);
  } catch (...) {
    parallelVersion = {0, 0, 0};
  }

  RetroArchVersion retroVersion;
  try {
    retroVersion = parse<RetroArchVersion>(json[P_RETROARCH]);
  } catch (...) {
    retroVersion = RetroArchVersion{Version{0, 0, 0}, false};
  }

  return InstalledVersionsInfo{retroVersion, parallelVersion,
                               parse<MupenCoreVersion>(json[P_MUPEN]),
                               json[P_LAST_CHECKED].get<int64>()};
}

template <>
ParallelCoreVersion
JsonSerializer::parse<ParallelCoreVersion>(const Json &json) {
#if defined(__linux__)
  const Json &vjson = json["linux_x64"];
#elif defined(_WIN32)
#ifdef _WIN64
  const Json &vjson = json["windows_x64"];
#else
  const Json &vjson = json["windows_x86"];
#endif
#elif defined(__APPLE__)
  const Json &vjson =
      AppleUtil::shouldUseArmCore() ? json["macos_arm64"] : json["macos_x64"];
#else
  const Json &vjson = json;
  static_assert(false);
#endif

  return ParallelCoreVersion{JsonSerializer::parse<Version>(vjson["version"]),
                             vjson["sha1"].get<string>(),
                             vjson["url"].get<string>()};
}

static void headAsync(QNetworkAccessManager *web, const string &url,
                      const std::function<void(void)> &onSuccess,
                      const std::function<void(void)> &onFailure) {
  QNetworkReply *response = web->head(QNetworkRequest(QUrl(url.c_str())));
  logDebug("HEAD "s + url);

  QObject::connect(
      response, ON_NETWORK_ERROR, web,
      [=](QNetworkReply::NetworkError error) {
        (void)error;
        response->deleteLater();
        onFailure();
      },
      Qt::QueuedConnection);

  QObject::connect(
      response, &QNetworkReply::finished, web,
      [=]() {
        if (response->error() != QNetworkReply::NoError)
          return;
        bool hasStatusCode = false;
        const int statusCode =
            response->attribute(QNetworkRequest::HttpStatusCodeAttribute)
                .toInt(&hasStatusCode);
        response->deleteLater();

        if (hasStatusCode && statusCode == 200) {
          onSuccess();
        } else {
          onFailure();
        }
      },
      Qt::QueuedConnection);
}

static void getAsync(QNetworkAccessManager *web, const string &url,
                     const std::function<void(const Json &)> &onSuccess,
                     const std::function<void(void)> &onFailure) {
  QNetworkReply *response = web->get(QNetworkRequest(QUrl(url.c_str())));
  logDebug("GET "s + url);

  QObject::connect(
      response, ON_NETWORK_ERROR, web,
      [=](QNetworkReply::NetworkError error) {
        (void)error;
        response->deleteLater();
        onFailure();
      },
      Qt::QueuedConnection);

  QObject::connect(
      response, &QNetworkReply::finished, web,
      [=]() {
        if (response->error() != QNetworkReply::NoError)
          return;
        const string responseBody = response->readAll().toStdString();
        response->deleteLater();

        std::stringstream jsonStream(responseBody);
        Json jsonBody;
        try {
          jsonBody = Json::parse(jsonStream);
        } catch (...) {
          onFailure();
          return;
        }

        onSuccess(jsonBody);
      },
      Qt::QueuedConnection);
}

#ifdef _WIN32
#ifdef _WIN64
static const string MUPEN_BUILD_NAME = "libretro-build-windows-x64-mingw";
#else
static const string MUPEN_BUILD_NAME = "libretro-build-windows-i686-mingw";
#endif
#else
#ifdef _LP64
static const string MUPEN_BUILD_NAME = "libretro-build-linux-x64";
#else
static const string MUPEN_BUILD_NAME = "libretro-build-linux-i686";
#endif
#endif

static void getBuildFromAnyPipelineAsync(
    QNetworkAccessManager *web, const string &baseUrl, const string &buildName,
    std::queue<int> *pipelines,
    const std::function<void(const string &)> &onSuccess,
    const std::function<void(void)> &onFailure) {
  if (pipelines->empty()) {
    delete pipelines;
    onFailure();
    return;
  }

  getAsync(
      web,
      baseUrl + "pipelines/"s + Number::toString(pipelines->front()) + "/jobs",
      [=](const Json &body) {
        try {
          for (const Json &job : body.array()) {
            if (job["name"].get<string>() == buildName) {
              const int jobId = job["id"].get<int>();
              const string downloadLink =
                  baseUrl + "jobs/"s + Number::toString(jobId) + "/artifacts";
              headAsync(
                  web, downloadLink,
                  [=]() {
                    delete pipelines;
                    onSuccess(downloadLink);
                  },
                  [=]() {
                    pipelines->pop();
                    getBuildFromAnyPipelineAsync(web, baseUrl, buildName,
                                                 pipelines, onSuccess,
                                                 onFailure);
                  });
              return;
            }
          }

          pipelines->pop();
          getBuildFromAnyPipelineAsync(web, baseUrl, buildName, pipelines,
                                       onSuccess, onFailure);
        } catch (...) {
          pipelines->pop();
          getBuildFromAnyPipelineAsync(web, baseUrl, buildName, pipelines,
                                       onSuccess, onFailure);
        }
      },
      [=]() {
        pipelines->pop();
        getBuildFromAnyPipelineAsync(web, baseUrl, buildName, pipelines,
                                     onSuccess, onFailure);
      });
}

static void
getDownloadLink(QNetworkAccessManager *web, const string &baseUrl,
                const string &commitHash, const string &targetBuildName,
                const std::function<void(const string &)> &onSuccess,
                const std::function<void(void)> &onFailure) {
  getAsync(
      web,
      baseUrl + "pipelines?scope=finished&status=success&sha="s + commitHash,
      [=](const Json &body) {
        try {
          std::queue<int> *builds = new std::queue<int>();
          for (const Json &buildJson : body.array()) {
            builds->push(buildJson["id"].get<int>());
          }
          getBuildFromAnyPipelineAsync(web, baseUrl, targetBuildName, builds,
                                       onSuccess, onFailure);
        } catch (...) {
          onFailure();
        }
      },
      onFailure);
}

static void getLKG(QNetworkAccessManager *web, const string &baseUrl,
                   std::queue<CommitInfo> *commits,
                   const string &targetBuildName,
                   const std::function<void(const CoreBuild &)> &onSuccess,
                   const std::function<void(void)> &onFailure) {
  if (commits->empty()) {
    onFailure();
    return;
  }

  getDownloadLink(
      web, baseUrl, commits->front().hash, targetBuildName,
      [=](const string &downloadLink) {
        onSuccess({std::move(commits->front()), downloadLink});
      },
      [=]() {
        commits->pop();
        getLKG(web, baseUrl, commits, targetBuildName, onSuccess, onFailure);
      });
}

#ifndef __APPLE__
void MupenCoreBuilds::getLastKnownGood(
    const string &branch, const string &laterThan,
    const std::function<void(const CoreBuild &)> &onSuccess,
    const std::function<void(void)> &onFailure) {
  const string baseUrl = "https://git.libretro.com/api/v4/projects/8/";

  QNetworkAccessManager *web = new QNetworkAccessManager();
  getAsync(
      web,
      baseUrl + "repository/commits?ref_name="s + branch + "&first_parent=1",
      [=](const Json &body) {
        std::queue<CommitInfo> *commits = new std::queue<CommitInfo>();
        try {
          for (const Json &json : body.array()) {
            string commitHash = json["id"].get<string>();
            if (commitHash == laterThan)
              break;
            commits->push({std::move(commitHash), json["title"].get<string>(),
                           json["created_at"].get<string>()});
          }
        } catch (...) {
          delete commits;
          web->deleteLater();
          onFailure();
          return;
        }

        getLKG(
            web, baseUrl, commits, MUPEN_BUILD_NAME,
            [=](const CoreBuild &lkg) {
              delete commits;
              web->deleteLater();
              onSuccess(lkg);
            },
            [=]() {
              delete commits;
              web->deleteLater();
              onFailure();
            });
      },
      [=]() {
        web->deleteLater();
        onFailure();
      });
}
#endif

static inline bool
checkSchedule(const InstalledVersionsInfo &installedVersions) {
  const QDate now = QDateTime::currentDateTime().date();
  const QDate lastUpdate =
      QDateTime::fromSecsSinceEpoch(installedVersions.lastUpdateCheck).date();

  if (now.year() != lastUpdate.year()) {
    return true;
  }

  switch (FileController::loadAppSettings().coreUpdateInterval) {
  case CoreUpdateInterval::EveryLaunch:
    return true;
  case CoreUpdateInterval::Daily:
    return now.dayOfYear() != lastUpdate.dayOfYear();
  case CoreUpdateInterval::Weekly:
    return now.weekNumber() != lastUpdate.weekNumber();
  case CoreUpdateInterval::Monthly:
    return now.month() != lastUpdate.month();
  default:
    return false;
  }
}

void ParallelCoreBuilds::getLatest(
    const std::function<void(const ParallelCoreVersion &)> &onSuccess,
    const std::function<void(void)> &onFailure) {
  QNetworkAccessManager *web = new QNetworkAccessManager();
  getAsync(
      web, "https://parallel-launcher.ca/latest/paralleln64/version.json",
      [=](const Json &body) {
        try {
          const ParallelCoreVersion version =
              JsonSerializer::parse<ParallelCoreVersion>(body);
          onSuccess(version);
          web->deleteLater();
        } catch (...) {
          web->deleteLater();
          onFailure();
        }
      },
      [=]() {
        web->deleteLater();
        onFailure();
      });
}

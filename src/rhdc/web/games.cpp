#include "src/rhdc/web/api-helpers.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/rhdc/web/require-login.hpp"

#include "src/core/logging.hpp"
#include "src/core/special-groups.hpp"
#include "src/core/time.hpp"
#include <QTimer>
#include <cctype>
#include <map>
#include <memory>

struct GroupNameJson {
  string data;
};

template <>
void JsonSerializer::serialize<GroupNameJson>(JsonWriter &jw,
                                              const GroupNameJson &obj) {
  std::ostringstream doubleJson;
  JsonWriter innerWriter(&doubleJson, false);
  innerWriter.writeString(obj.data);

  jw.writeObjectStart();
  jw.writeProperty("data", doubleJson.str());
  jw.writeObjectEnd();
}

static inline string listToGroup(const string &listName) {
  if (listName == "Want To Play") {
    return SpecialGroups::WantToPlay;
  } else if (listName == "In Progress") {
    return SpecialGroups::InProgress;
  } else if (listName == "Completed") {
    return SpecialGroups::Completed;
  } else if (listName == "Favorites") {
    return SpecialGroups::Favourites;
  } else
    return listName;
}

static inline std::optional<GfxPlugin> parsePlugin(const Json &jString) {
  static const std::map<string, GfxPlugin> tokens = {
      {"ParaLLEl", GfxPlugin::ParaLLEl},   {"GLideN64", GfxPlugin::GLideN64},
      {"Glide64", GfxPlugin::Glide64},     {"Rice", GfxPlugin::Rice},
      {"Angrylion", GfxPlugin::Angrylion}, {"OGRE", GfxPlugin::OGRE}};

  const auto p = tokens.find(jString.getOrDefault<string>(""));
  if (p == tokens.end())
    return std::nullopt;
  return p->second;
}

static inline PluginFlags parsePluginFlags(const Json &jArray) {
  static const std::map<string, PluginFlags> tokens = {
      {"upscale-texrects", PluginFlags::UpscaleTexrects},
      {"allow-hle-fallback", PluginFlags::AllowHleFallback},
      {"emulate-framebuffer", PluginFlags::EmulateFramebuffer},
      {"accurate-depth-compare", PluginFlags::AcccurateDepthCompare},
      {"widescreen", PluginFlags::Widescreen},
      {"lle-rsp", PluginFlags::EmulateRsp}};

  PluginFlags flags = PluginFlags::None;
  if (!jArray.isArray())
    return flags;
  for (const Json &jString : jArray.array()) {
    const auto f = tokens.find(jString.getOrDefault<string>(""));
    if (f == tokens.end())
      continue;
    flags |= f->second;
  }

  return flags;
}

static inline RhdcHackFlag parseHackFlags(const Json &jArray) {
  RhdcHackFlag hackFlags = RhdcHackFlag::None;
  if (jArray.isArray()) {
    for (const Json &flagJson : jArray.array()) {
      const string flag = flagJson.getOrDefault<string>("");
      if (flag == "no-overclock") {
        hackFlags |= RhdcHackFlag::NoOverclock;
      } else if (flag == "dual-analog") {
        hackFlags |= RhdcHackFlag::DualAnalog;
      } else if (flag == "big-eeprom") {
        hackFlags |= RhdcHackFlag::BigEEPROM;
      } else if (flag == "sd-card") {
        hackFlags |= RhdcHackFlag::SupportsSD;
      }
    }
  }
  return hackFlags;
}

static inline ubyte fromHex(char c) {
  if (c >= '0' && c <= '9')
    return (ubyte)(c - '0');
  if (c >= 'a' && c <= 'f')
    return (ubyte)(c - 'a' + 10);
  if (c >= 'A' && c <= 'F')
    return (ubyte)(c - 'A' + 10);
  return 0;
}

static string urlDecode(const string &url) {
  string decoded;
  decoded.reserve(url.size());
  for (size_t i = 0; i < url.size(); i++) {
    if (url[i] == '%' && i < url.size() + 2 &&
        std::isxdigit(static_cast<ubyte>(url[i + 1])) &&
        std::isxdigit(static_cast<ubyte>(url[i + 2]))) {
      ubyte c = fromHex(url[++i]) << 4;
      c |= fromHex(url[++i]);
      decoded += (char)c;
    } else {
      decoded += url[i];
    }
  }

  return decoded;
}

static inline string tryGetDownloadUrl(const Json &token) {
  if (!token.exists() || token.isNull())
    return "";
  return string(RHDC_API_HOST) +
         urlDecode(token["directHref"].getOrDefault<string>(""));
}

static string tryGetVersionName(const Json &token) {
  const string versionUrl = tryGetDownloadUrl(token);
  if (versionUrl.empty())
    return "";

  const auto namePos = versionUrl.find_last_of('/');
  if (namePos == string::npos)
    return versionUrl;

  return versionUrl.substr(namePos + 1);
}

static std::vector<RhdcHackVersionExt>
parseVersions(const JArray &versionsJson) {
  std::vector<RhdcHackVersionExt> versions;
  versions.reserve(versionsJson.size());
  for (size_t i = 0; i < versionsJson.size(); i++) {
    const Json &version = versionsJson.at(versionsJson.size() - 1 - i);
    string sha1 = version["patchedSha1"].getOrDefault<string>("");
    if (sha1.empty())
      continue;

    string versionName = tryGetVersionName(version["download"]);
    if (versionName.empty())
      continue;

    versions.push_back({std::move(sha1), std::move(versionName),
                        parsePlugin(version["plugin"]),
                        parsePluginFlags(version["pluginFlags"]),
                        parseHackFlags(version["hackFlags"]),
                        version["archived"].getOrDefault<bool>(false)});
  }

  return versions;
}

void RhdcApi::getFollowedHacksAsync(
    const std::function<void(HashMap<string, FollowedHack> &)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN([=]() { getFollowedHacksAsync(onSuccess, onFailure); })
  QNetworkReply *response =
      ApiUtil::send(HttpMethod::Get, RHDC_API_HOST "/v3/hacks/following");
  ApiUtil::onResponse(
      response,
      [=]() {
        const Json json = ApiUtil::readJson(response);
        if (!json.isArray()) {
          logError(
              "Unexpected JSON format in response from RHDC following query");
          onFailure(ApiErrorType::JsonError);
          return;
        }

        HashMap<string, FollowedHack> hackMap;
        const JArray &followedHacksJson = json.array();
        for (const Json &hack : followedHacksJson) {
          const std::vector<RhdcHackVersionExt> versions =
              parseVersions(hack["versions"].array());
          if (versions.empty())
            continue;

          std::vector<string> groups;
          const JArray &groupsJson = hack["playlists"].array();
          groups.reserve(groupsJson.size());
          for (const Json &group : groupsJson) {
            groups.push_back(listToGroup(group.get<string>()));
          }

          std::vector<string> authors;
          const JArray &authorsJson = hack["authors"].array();
          authors.reserve(authorsJson.size());
          for (const Json &author : authorsJson) {
            authors.push_back(author["username"].get<string>());
          }

          int64 layoutUpdated = 0;
          const string layoutUpdatedStr =
              hack["layoutUpdateTime"].getOrDefault<string>("");
          if (!layoutUpdatedStr.empty()) {
            bool validDate = false;
            layoutUpdated = Time::tryParseIso(layoutUpdatedStr, validDate);
            if (!validDate)
              layoutUpdated = 0;
          }

          const string hackId = hack["hackId"].get<string>();
          string layoutUrl = tryGetDownloadUrl(hack["layout"]);

          Starpower starpower = {{RhdcRating::NotRated, 0, 0, false}, 0};
          const Json &starpowerJson = hack["progress"];
          if (starpowerJson.exists() && !starpowerJson.isNull()) {
            starpower.playTime =
                starpowerJson["playTime"].getOrDefault<int64>(0);
            starpower.progress.stars =
                starpowerJson["claimedStarPoints"].getOrDefault<ushort>(0);
            starpower.progress.completed =
                starpowerJson["claimedHackComplete"].getOrDefault<bool>(false);
            starpower.progress.rating =
                JsonSerializer::parse<RhdcRating>(starpowerJson["rating"]);
            starpower.progress.difficulty =
                starpowerJson["difficulty"].getOrDefault<ubyte>(0);
          }

          hackMap[hackId] = FollowedHack{
              RhdcHackInfo{
                  hackId, hack["title"].get<string>(),
                  hack["urlTitle"].get<string>(),
                  hack["description"].get<string>(),
                  hack["stars"].getOrDefault<int>(0),
                  hack["numDownloads"].getOrDefault<int>(0),
                  hack["rating"].getOrDefault<double>(0.0),
                  hack["difficulty"].getOrDefault<double>(0.0),
                  hack["category"].get<string>(),
                  !hack["needsVerification"].getOrDefault<bool>(false)},
              starpower,
              std::move(versions),
              std::move(groups),
              std::move(authors),
              std::move(layoutUrl),
              layoutUpdated};
        }

        onSuccess(hackMap);
      },
      [response, onFailure](ApiErrorType err) {
        if (err != ApiErrorType::NotAuthorized) {
          ApiUtil::logResponseError(response);
        }
        onFailure(err);
      });
}

void RhdcApi::getHackAsync(
    const string &hackSlug,
    const std::function<void(RhdcVersionedHack &)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  OPTIONAL_LOGIN([=]() { getHackAsync(hackSlug, onSuccess, onFailure); })
  QNetworkReply *response = ApiUtil::send(
      HttpMethod::Get, string(RHDC_API_HOST "/v3/hacks/hack/") + hackSlug);
  ApiUtil::onResponse(
      response,
      [=]() {
        const Json json = ApiUtil::readJson(response);
        if (!json.isObject() || !json["versions"].isArray()) {
          logError("Unexpected JSON format in response from RHDC game query");
          onFailure(ApiErrorType::JsonError);
          return;
        }

        int64 layoutUpdated = 0;
        const string layoutUpdatedStr =
            json["layoutUpdateTime"].getOrDefault<string>("");
        if (!layoutUpdatedStr.empty()) {
          bool validDate = false;
          layoutUpdated = Time::tryParseIso(layoutUpdatedStr, validDate);
          if (!validDate)
            layoutUpdated = 0;
        }

        const string hackId = json["hackId"].get<string>();
        const string layoutUrl = tryGetDownloadUrl(json["layout"]);

        std::vector<string> authors;
        if (json["authors"].isArray()) {
          const JArray &authorsJson = json["authors"].array();
          authors.reserve(authorsJson.size());
          for (const Json &author : authorsJson) {
            authors.push_back(author["username"].get<string>());
          }
        }

        RhdcVersionedHack hack = {
            RhdcHackInfo{hackId, json["title"].get<string>(),
                         json["urlTitle"].get<string>(),
                         json["description"].get<string>(),
                         json["stars"].getOrDefault<int>(0),
                         json["numDownloads"].getOrDefault<int>(0),
                         json["rating"].getOrDefault<double>(0.0),
                         json["difficulty"].getOrDefault<double>(0.0),
                         json["category"].get<string>(),
                         !json["needsVerification"].getOrDefault<bool>(false)},
            parseVersions(json["versions"].array()), std::move(authors),
            layoutUrl, layoutUpdated};

        onSuccess(hack);
      },
      onFailure);
}

void RhdcApi::getLayoutAndRecommendedPluginAsync(
    const string &hackId, const string &versionName,
    const std::function<void(PluginAndLayoutInfo &)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  OPTIONAL_LOGIN([=]() {
    getLayoutAndRecommendedPluginAsync(hackId, versionName, onSuccess,
                                       onFailure);
  })
  QNetworkReply *response = ApiUtil::send(
      HttpMethod::Get, string(RHDC_API_HOST "/v3/hacks/hack/") + hackId);
  ApiUtil::onResponse(
      response,
      [=]() {
        const Json json = ApiUtil::readJson(response);
        if (!json.isObject() || !json["versions"].isArray()) {
          logError("Unexpected JSON format in response from RHDC game query");
          onFailure(ApiErrorType::JsonError);
          return;
        }

        string layoutUrl = tryGetDownloadUrl(json["layout"]);

        const JArray versionsJson = json["versions"].array();
        for (const Json &version : versionsJson) {
          if (tryGetVersionName(version["download"]) != versionName)
            continue;

          PluginAndLayoutInfo info = {parsePlugin(version["plugin"]),
                                      parsePluginFlags(version["pluginFlags"]),
                                      std::move(layoutUrl)};

          onSuccess(info);
          return;
        }

        onFailure(ApiErrorType::NotFound);
      },
      onFailure);
}

void RhdcApi::addHackToListAsync(
    const string &hackId, const string &group,
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN(
      [=]() { addHackToListAsync(hackId, group, onSuccess, onFailure); })
  QNetworkReply *response = ApiUtil::send(
      HttpMethod::Post, string(RHDC_API_HOST "/v3/playlists/name/") +
                            ApiUtil::urlEncode(group) + "/hack/" + hackId);
  ApiUtil::onResponse(response, onSuccess, onFailure);
}

void RhdcApi::removeHackFromListAsync(
    const string &hackId, const string &group,
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN(
      [=]() { removeHackFromListAsync(hackId, group, onSuccess, onFailure); })
  QNetworkReply *response = ApiUtil::send(
      HttpMethod::Delete, string(RHDC_API_HOST "/v3/playlists/name/") +
                              ApiUtil::urlEncode(group) + "/hack/" + hackId);
  ApiUtil::onResponse(response, onSuccess, onFailure);
}

void RhdcApi::deleteListAsync(
    const string &group, const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN([=]() { deleteListAsync(group, onSuccess, onFailure); })
  QNetworkReply *response = ApiUtil::send(
      HttpMethod::Delete,
      string(RHDC_API_HOST "/v3/playlists/name/") + ApiUtil::urlEncode(group));
  ApiUtil::onResponse(response, onSuccess, onFailure);
}

void RhdcApi::downloadHack(const string &downloadUrl, QFile *destinationFile,
                           const std::function<void(int64, int64)> &onProgress,
                           const std::function<void(void)> &onSuccess,
                           const std::function<void(ApiErrorType)> &onFailure) {
  OPTIONAL_LOGIN([=]() {
    downloadHack(downloadUrl, destinationFile, onProgress, onSuccess,
                 onFailure);
  })
  QNetworkReply *response = ApiUtil::send(HttpMethod::Get, downloadUrl);

  QObject::connect(response, &QNetworkReply::readyRead, ApiUtil::web(),
                   [=]() { destinationFile->write(response->readAll()); });

  QObject::connect(response, &QNetworkReply::downloadProgress, ApiUtil::web(),
                   [=](qint64 downloaded, qint64 total) {
                     onProgress((int64)downloaded, (int64)total);
                   });

  ApiUtil::onResponse(response, onSuccess, onFailure);
}

void RhdcApi::downloadThumbnail(
    const string &hackId, QFile *destinationFile,
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  OPTIONAL_LOGIN([=]() {
    downloadThumbnail(hackId, destinationFile, onSuccess, onFailure);
  })
  QNetworkReply *response = ApiUtil::send(
      HttpMethod::Get, string(RHDC_API_HOST "/v3/hacks/hack/") + hackId);
  ApiUtil::onResponse(
      response,
      [=]() {
        const Json json = ApiUtil::readJson(response);
        if (!json["screenshots"].isArray()) {
          onFailure(ApiErrorType::JsonError);
          return;
        }

        const JArray screenshots = json["screenshots"].array();
        if (screenshots.empty()) {
          onFailure(ApiErrorType::NotFound);
          return;
        }

        const string screenshotHref =
            screenshots[0]["directHref"].getOrDefault<string>("");
        if (screenshotHref.empty()) {
          onFailure(ApiErrorType::NotFound);
          return;
        }

        QNetworkReply *download = ApiUtil::send(
            HttpMethod::Get, string(RHDC_API_HOST) + screenshotHref);
        QObject::connect(
            download, &QNetworkReply::readyRead, ApiUtil::web(),
            [=]() { destinationFile->write(download->readAll()); });

        ApiUtil::onResponse(download, onSuccess, onFailure);
      },
      onFailure);
}

static void checksumListSerializer(JsonWriter &jw,
                                   const std::vector<string> &checksums) {
  jw.writeArrayStart();
  for (const string &sha1 : checksums) {
    jw.writeString(sha1);
  }
  jw.writeArrayEnd();
}

void RhdcApi::getPluginSettings(
    const std::vector<string> &checksums,
    const std::function<void(const std::vector<RhdcSettingHintsExt> &)>
        &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure, int timeoutMs) {
  QNetworkReply *response =
      ApiUtil::send(HttpMethod::Post, string(RHDC_API_HOST "/v3/hacks/plugins"),
                    checksums, checksumListSerializer);

  std::shared_ptr<bool> isAlive(new bool(true));
  ApiUtil::onResponse(
      response,
      [=]() {
        *isAlive = false;

        const Json json = ApiUtil::readJson(response);
        if (!json.isArray()) {
          logError("Unexpected JSON format in response from RHDC game query");
          onFailure(ApiErrorType::JsonError);
          return;
        }

        std::vector<RhdcSettingHintsExt> results;
        for (const Json &info : json.array()) {
          const string &sha1 = info["sha1"].getOrDefault<string>("");
          if (sha1.empty())
            continue;
          results.push_back({sha1, parsePlugin(info["plugin"]),
                             parsePluginFlags(info["pluginFlags"]),
                             parseHackFlags(info["hackFlags"])});
        }

        onSuccess(results);
      },
      [=](ApiErrorType errorType) {
        *isAlive = false;
        onFailure(errorType);
      });

  QTimer::singleShot(timeoutMs, [=]() {
    if (!*isAlive)
      return;
    *isAlive = false;
    response->abort();
  });
}

extern void RhdcApi::getPluginSettings(
    const string &sha1,
    const std::function<void(const std::optional<RhdcSettingHintsExt> &)>
        &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure, int timeoutMs) {
  RhdcApi::getPluginSettings(
      std::vector<string>{sha1},
      [=](const std::vector<RhdcSettingHintsExt> &results) {
        if (results.empty()) {
          onSuccess(std::optional<RhdcSettingHintsExt>(std::nullopt));
        } else {
          onSuccess(std::optional<RhdcSettingHintsExt>(results[0]));
        }
      },
      onFailure, timeoutMs);
}

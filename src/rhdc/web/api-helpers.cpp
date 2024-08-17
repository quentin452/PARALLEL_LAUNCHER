#include "src/rhdc/web/api-helpers.hpp"

#include "src/core/logging.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/steam-deck.hpp"
#include "src/core/version.hpp"
#include <QNetworkInterface>
#include <QSysInfo>
#include <cstdio>

#ifdef FLATPAK_VERSION
#include "src/polyfill/file.hpp"
#include <stdlib.h>
#endif

#ifdef _WIN32
#include "src/polyfill/process.hpp"
#endif

#ifdef __APPLE__
#include "src/polyfill/macos/apple-util.hpp"
#endif

using namespace ApiUtil;

#define DECLARE_VERB(verb, verbString)                                         \
  static const QByteArray &s_##verb() {                                        \
    static const QByteArray s_verb = QByteArray(verbString);                   \
    return s_verb;                                                             \
  }                                                                            \
  const HttpMethodFactory HttpMethod::verb = s_##verb;

DECLARE_VERB(Get, "GET")
DECLARE_VERB(Post, "POST")
DECLARE_VERB(Put, "PUT")
DECLARE_VERB(Patch, "PATCH")
DECLARE_VERB(Delete, "DELETE")
DECLARE_VERB(Head, "HEAD")

#undef DECLARE_VERB

QNetworkAccessManager *ApiUtil::web() {
  static QNetworkAccessManager *s_web = new QNetworkAccessManager();
  return s_web;
}

__Identity &ApiUtil::identity() noexcept {
  static __Identity s_identity = {"", "", QByteArray()};
  return s_identity;
}

inline bool isUrlSafe(char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') || c == '-' || c == '.' || c == '_' || c == '~';
}

string ApiUtil::urlEncode(const string &str) {
  string safeString;
  for (char c : str) {
    if (isUrlSafe(c)) {
      safeString += c;
    } else {
      char code[4];
      std::sprintf(code, "%%%02x", (uint)(ubyte)c);
      code[3] = '\0';
      safeString += code;
    }
  }
  return safeString;
}

#if defined(_WIN32)
#define PL_GENERIC_OS_NAME "Windows"
#elif defined(__APPLE__)
#define PL_GENERIC_OS_NAME "MacOS"
#else
#define PL_GENERIC_OS_NAME "Linux"
#endif

static inline QString sanitizeUserAgentField(const QString &str) {
  return QString::fromUtf8(QUrl::toPercentEncoding(str, " +"));
}

struct OSInfo {
  QString name;
  QString version;
};

static inline OSInfo getOS() {
#if defined(FLATPAK_VERSION)
  InputFile osInfo("/run/host/etc/os-release", false);
  QString osName, osVersion;

  string line;
  while (std::getline(osInfo, line).good() && !osInfo.eof()) {
    if (line.substr(0, 3) == "ID=") {
      osName = QString(line.substr(3).c_str());
    } else if (line.substr(0, 11) == "VERSION_ID=") {
      osVersion = QString(line.substr(11).c_str());
    }
  }

  if (osName.isNull() || osName.isEmpty()) {
    osName = QSysInfo::productType();
  }

  if (osVersion.isNull() || osVersion.isEmpty()) {
    osVersion = QSysInfo::productVersion();
  }

  return OSInfo{osName, osVersion};
#elif defined(_WIN32)
  const QString osName = QSysInfo::productType();
  QString osVersion = QSysInfo::productVersion();

  if (osVersion == "10") {
    string queryResult;
    if (Process::tryGetOutput(
            "wmic /LOCALE:MS_409 path win32_operatingsystem get caption",
            queryResult)) {
      if (queryResult.find("Microsoft Windows 11") != std::string::npos) {
        osVersion = QString("11");
      }
    }
  }

  return OSInfo{osName, osVersion};
#else
  return OSInfo{QSysInfo::productType(), QSysInfo::productVersion()};
#endif
}

static QString getUserAgentHeader() {
  QString userAgent = QString("ParallelLauncher/");
  OSInfo osInfo = getOS();
  userAgent += CurrentVersion::Application.toString().c_str();
  userAgent += " (";
  userAgent += PL_GENERIC_OS_NAME " ";
#ifdef __APPLE__
  if (AppleUtil::isSilicon()) {
    userAgent += "arm64";
  } else {
    userAgent += sanitizeUserAgentField(QSysInfo::buildCpuArchitecture());
  }
#else
  userAgent += sanitizeUserAgentField(QSysInfo::buildCpuArchitecture());
#endif
  userAgent += "; ";
  userAgent += sanitizeUserAgentField(osInfo.name);
  userAgent += "/";
  userAgent += sanitizeUserAgentField(osInfo.version);
#ifdef FLATPAK_VERSION
  userAgent += "; Flatpak";
#endif
  if (SteamDeck::isSteamDeck()) {
    userAgent += "; Steam Deck in ";
    userAgent += SteamDeck::inGamingMode() ? "Gaming Mode" : "Desktop Mode";
  }
  userAgent += ")";

  return userAgent;
}

QNetworkRequest ApiUtil::makeRequest(const string &url, const char *contentType,
                                     bool tryAuthorize) {
  static const QVariant s_userAgent = QVariant::fromValue(getUserAgentHeader());

  QNetworkRequest request(QUrl(url.c_str()));
  request.setHeader(QNetworkRequest::UserAgentHeader, s_userAgent);

  if (contentType != nullptr) {
    const QVariant s_contentType = QVariant::fromValue<QString>(contentType);
    request.setHeader(QNetworkRequest::ContentTypeHeader, s_contentType);
  }

  if (tryAuthorize && !identity().authToken.isNull()) {
    static const QByteArray s_headerName = QByteArray("Authorization");
    request.setRawHeader(s_headerName, identity().authToken);
  }

#ifdef RHDC_DEV_KEY
  static const QByteArray s_devKeyHeaderName = QByteArray("X-Dev-Key");
  static const QByteArray s_devKeyHeaderValue = QByteArray(RHDC_DEV_KEY);
  request.setRawHeader(s_devKeyHeaderName, s_devKeyHeaderValue);
#endif

  return request;
}

int ApiUtil::getStatusCode(QNetworkReply *response) {
  bool hasStatusCode;
  const int statusCode =
      response->attribute(QNetworkRequest::HttpStatusCodeAttribute)
          .toInt(&hasStatusCode);
  return hasStatusCode ? statusCode : 0;
}

Json ApiUtil::readJson(QNetworkReply *response) noexcept {
  try {
    std::istringstream stream(response->readAll().data());
    return Json::parse(stream);
  } catch (...) {
    return Json();
  }
}

void ApiUtil::onResponse(QNetworkReply *response,
                         const std::function<void(void)> &onSuccess,
                         const std::function<void(ApiErrorType)> &onFailure) {
  QObject::connect(
      response, &QNetworkReply::finished, response->manager(),
      [=]() {
        ApiErrorType errorType;
        bool good = false;

        const HttpStatusCode statusCode =
            (HttpStatusCode)getStatusCode(response);
        logDebug(Number::toString((int)statusCode) + ' ' +
                 response->url().toString().toStdString());

        switch (statusCode) {
        case HttpStatusCode::OK:
        case HttpStatusCode::Created:
        case HttpStatusCode::Accepted:
        case HttpStatusCode::NoContent:
          good = true;
          break;
        case HttpStatusCode::Unauthorized:
        case HttpStatusCode::Forbidden:
          errorType = ApiErrorType::NotAuthorized;
          break;
        case HttpStatusCode::NotFound:
        case HttpStatusCode::Gone:
          errorType = ApiErrorType::NotFound;
          break;
        case HttpStatusCode::TooManyRequests:
        case HttpStatusCode::InternalServerError:
        case HttpStatusCode::BadGateway:
        case HttpStatusCode::ServiceUnavailable:
        case HttpStatusCode::GatewayTimeout:
        case HttpStatusCode::None:
          errorType = ApiErrorType::ServerDown;
          break;
        default:
          errorType = ApiErrorType::UnknownError;
        }

        if (statusCode == HttpStatusCode::None) {
          errorType = ApiUtil::isOnline() ? ApiErrorType::ServerDown
                                          : ApiErrorType::Offline;
        }

        good ? onSuccess() : onFailure(errorType);
        response->deleteLater();
      },
      Qt::QueuedConnection);
}

void ApiUtil::logResponseError(QNetworkReply *response) {
  const int statusCode = getStatusCode(response);
  const Json json = readJson(response);

  const string url = response->url().toString().toStdString();
  const string title = json["title"].getOrDefault<string>("Unknown");
  const string detail = json["detail"].getOrDefault<string>("(none)");

  const string msg = "Unexpected API error response."s + "\nURL: " + url +
                     "\nStatus Code: " + Number::toString(statusCode) +
                     "\nError Type: " + title + "\nError Detail: " + detail;

  logError(msg);
}

bool ApiUtil::isOnline() {
  for (const QNetworkInterface &network : QNetworkInterface::allInterfaces()) {
    if (network.flags().testFlag(QNetworkInterface::IsRunning) &&
        !network.flags().testFlag(QNetworkInterface::IsLoopBack))
      return true;
  }
  return false;
}

std::function<void(ApiErrorType)> RhdcApi::logApiError(const string &message) {
  return [message](ApiErrorType errType) {
    switch (errType) {
    case ApiErrorType::NotAuthorized:
      logWarn(message + " [Not Authorized]");
      break;
    case ApiErrorType::NotFound:
      logError(message + " [Not Found]");
      break;
    case ApiErrorType::ServerDown:
      logError(message + " [Server Outage]");
      break;
    case ApiErrorType::JsonError:
      logError(message + " [JSON Error]");
      break;
    case ApiErrorType::UnknownError:
      logError(message + " [Unknown Error]");
      break;
    case ApiErrorType::Offline:
    case ApiErrorType::MfaCodeRequired:
      break;
    }
  };
}

string RhdcApi::getDownloadUrl(const string &slug, const string &versionName) {
  return string(RHDC_API_HOST "/game/") + slug + '/' +
         ApiUtil::urlEncode(versionName);
}

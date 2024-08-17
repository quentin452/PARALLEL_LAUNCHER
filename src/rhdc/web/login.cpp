#include "src/core/logging.hpp"
#include "src/core/time.hpp"
#include "src/rhdc/core/credentials.hpp"
#include "src/rhdc/web/api-helpers.hpp"
#include "src/rhdc/web/api.hpp"
#include <QTimer>
#include <mutex>
#include <vector>

bool RhdcApi::isAuthenticated() noexcept {
  return !ApiUtil::identity().authToken.isNull() &&
         !ApiUtil::identity().username.empty();
}

struct CredentialsJson {
  string username;
  string password;
  int mfaCode;
};

template <>
void JsonSerializer::serialize<CredentialsJson>(JsonWriter &jw,
                                                const CredentialsJson &obj) {
  jw.writeObjectStart();
  jw.writeProperty("username", obj.username);
  jw.writeProperty("password", obj.password);
  jw.writeProperty("application", "ParallelLauncher"s);
  if (obj.mfaCode >= 0) {
    jw.writeProperty("mfaCode", obj.mfaCode);
  }
  jw.writeObjectEnd();
}

struct VerifyLoginHandler {
  std::function<void(void)> onSuccess;
  std::function<void(ApiErrorType)> onFailure;
};

static std::mutex s_verifyLoginLock;
static std::vector<VerifyLoginHandler> s_verifyLoginHandlers;

static void verifyLoginSuccess() {
  s_verifyLoginLock.lock();
  const std::vector<VerifyLoginHandler> handlers =
      std::move(s_verifyLoginHandlers);
  s_verifyLoginLock.unlock();

  for (const VerifyLoginHandler &handler : handlers) {
    handler.onSuccess();
  }
}

static void verifyLoginFailure(ApiErrorType error) {
  s_verifyLoginLock.lock();
  const std::vector<VerifyLoginHandler> handlers =
      std::move(s_verifyLoginHandlers);
  s_verifyLoginLock.unlock();

  for (const VerifyLoginHandler &handler : handlers) {
    handler.onFailure(error);
  }
}

static void refreshTokenAsync() {
  QNetworkReply *response =
      ApiUtil::send(HttpMethod::Post, RHDC_API_HOST "/v3/auth/refresh");
  ApiUtil::onResponse(
      response,
      [=]() {
        const Json json = ApiUtil::readJson(response);
        const string jwt = json["token"].getOrDefault<string>("");

        if (jwt.empty()) {
          verifyLoginSuccess();
          return;
        }

        ApiUtil::identity().authToken = QByteArray(("Bearer "s + jwt).c_str());
        RhdcCredentials{ApiUtil::identity().username,
                        ApiUtil::identity().authToken.toStdString(), ""s}
            .save();

        verifyLoginSuccess();
      },
      [=](ApiErrorType) {
        logWarn("Failed to refresh token. Will try again next time.");
        verifyLoginSuccess();
      });
}

void RhdcApi::verifyLoginAsync(
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  s_verifyLoginLock.lock();
  if (!s_verifyLoginHandlers.empty()) {
    s_verifyLoginHandlers.push_back({onSuccess, onFailure});
    s_verifyLoginLock.unlock();
    return;
  }

  s_verifyLoginHandlers.push_back({onSuccess, onFailure});
  s_verifyLoginLock.unlock();

  QNetworkReply *response =
      ApiUtil::send(HttpMethod::Get, RHDC_API_HOST "/v3/auth/whoami");
  ApiUtil::onResponse(
      response,
      [response]() {
        const Json json = ApiUtil::readJson(response);
        const string userId = json["userId"].getOrDefault<string>("");
        if (userId.empty()) {
          RhdcApi::logout();
          verifyLoginFailure(ApiErrorType::JsonError);
        } else {
          ApiUtil::identity().userId = userId;
          ApiUtil::identity().username =
              json["username"].getOrDefault<string>("");

          const string dateString =
              json["tokenExpires"].getOrDefault<string>("");
          if (!dateString.empty()) {
            bool validDate = false;
            const int64 expiryDate = Time::tryParseIso(dateString, validDate);
            if (validDate && expiryDate - Time::now() < 15552000l) {
              refreshTokenAsync();
              return;
            }
          }

          RhdcCredentials{ApiUtil::identity().username,
                          ApiUtil::identity().authToken.toStdString(), ""s}
              .save();

          verifyLoginSuccess();
        }
      },
      [](ApiErrorType err) {
        if (err != ApiErrorType::ServerDown && err != ApiErrorType::Offline) {
          RhdcApi::logout();
        }
        verifyLoginFailure(err);
      });
}

void RhdcApi::loginAsync(const string &username, const string &password,
                         int mfaCode, bool awaitRetryAfter,
                         const std::function<void(void)> &onSuccess,
                         const std::function<void(ApiErrorType)> &onFailure) {
  QNetworkReply *response = ApiUtil::send<CredentialsJson>(
      HttpMethod::Post, RHDC_API_HOST "/v3/auth/login",
      CredentialsJson{username, password, mfaCode});
  ApiUtil::onResponse(
      response,
      [=]() {
        const Json json = ApiUtil::readJson(response);
        const string token = json["token"].getOrDefault<string>("");
        if (token.empty()) {
          logError("Failed to parse response body after calling login route.");
          onFailure(ApiErrorType::JsonError);
        } else {
          ApiUtil::identity().username = username;
          ApiUtil::identity().authToken =
              QByteArray(("Bearer "s + token).c_str());
          RhdcApi::verifyLoginAsync(onSuccess, onFailure);
        }
      },
      [=](ApiErrorType err) {
        if (err != ApiErrorType::ServerDown && err != ApiErrorType::Offline) {
          RhdcApi::logout();
        }

        const int statusCode = ApiUtil::getStatusCode(response);
        if (mfaCode < 0 && statusCode == (int)HttpStatusCode::Unauthorized &&
            QString::fromUtf8(
                response->rawHeader(QString("WWW-Authenticate").toUtf8()))
                .contains("mfa=required")) {
          onFailure(ApiErrorType::MfaCodeRequired);
          return;
        }

        if (awaitRetryAfter &&
            response->hasRawHeader(QByteArray::fromStdString("Retry-After"s))) {
          const Json json = ApiUtil::readJson(response);
          const int delay = json["retryAfterMs"].getOrDefault<int>(0);
          if (delay > 0) {
            QTimer::singleShot(delay, [onFailure]() {
              onFailure(ApiErrorType::NotAuthorized);
            });
            return;
          }
        }

        if (statusCode == (int)HttpStatusCode::BadRequest ||
            statusCode == (int)HttpStatusCode::Unauthorized) {
          onFailure(ApiErrorType::NotAuthorized);
        } else {
          if (err != ApiErrorType::ServerDown && err != ApiErrorType::Offline) {
            ApiUtil::logResponseError(response);
          }
          onFailure(err);
        }
      });
}

void RhdcApi::logout() noexcept {
  ApiUtil::identity().username.clear();
  ApiUtil::identity().userId.clear();
  ApiUtil::identity().authToken.clear();
  RhdcCredentials::forget();
}

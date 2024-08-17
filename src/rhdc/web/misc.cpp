#include "src/rhdc/web/api-helpers.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/rhdc/web/require-login.hpp"

void RhdcApi::getAvatarAsync(
    const string &username,
    const std::function<void(const QByteArray &)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  QNetworkReply *response =
      ApiUtil::send(HttpMethod::Get,
                    string(RHDC_API_HOST "/v3/user/") + username + "/avatar");
  ApiUtil::onResponse(
      response, [=]() { onSuccess(response->readAll()); }, onFailure);
}

void RhdcApi::downloadFile(const string &downloadUrl, QFile *destinationFile,
                           const std::function<void(int64, int64)> &onProgress,
                           const std::function<void(void)> &onSuccess,
                           const std::function<void(ApiErrorType)> &onFailure) {
  OPTIONAL_LOGIN([=]() {
    downloadFile(downloadUrl, destinationFile, onProgress, onSuccess,
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

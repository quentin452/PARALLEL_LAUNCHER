#include "src/rhdc/web/api-helpers.hpp"
#include "src/rhdc/web/api.hpp"

#include "src/core/app-context.hpp"
#include "src/core/logging.hpp"
#include "src/core/time.hpp"
#include "src/db/data-provider.hpp"
#include "src/polyfill/base-directory.hpp"
#include <QCoreApplication>
#include <QTimer>
#include <atomic>
#include <memory>

struct __telemetry_t {};

static inline QByteArray serializeTelemetryData() {
  std::ostringstream stream;
  JsonWriter jw(&stream, false);

  const AppContextInfo &context = getAppContext();

  jw.writeObjectStart();
  jw.writeProperty("id", context.deviceId);
  jw.writeProperty("resolutionWidth", context.screenWidth);
  jw.writeProperty("resolutionHeight", context.screenHeight);
  jw.writeProperty("dpiX", context.screenDpiX);
  jw.writeProperty("dpiY", context.screenDpiY);
  jw.writeProperty("locale", context.locale);
  jw.writeProperty("romCount", DataProvider::countRomFiles());
  jw.writeProperty("rhdcIntegrationEnabled",
                   RhdcCredentials::exists() || DataProvider::hasRhdcData());
  jw.writeObjectEnd();

  return QByteArray(stream.str().c_str());
}

void RhdcApi::sendTelemetryDatum() {
  QNetworkReply *response =
      ApiUtil::send(HttpMethod::Post, RHDC_API_HOST "/pl/telemetry",
                    "application/json", serializeTelemetryData(), false);
  ApiUtil::onResponse(
      response, []() {}, [](ApiErrorType) {});
}

void RhdcApi::sendCrashReports() {
  std::shared_ptr<bool> requestInFlight(new bool(true));

  std::map<int64, string> reports = DataProvider::getUnreportedCrashLogs();
  if (reports.empty())
    return;

  std::shared_ptr<std::atomic<intptr_t>> pendingCount =
      std::make_shared<std::atomic<intptr_t>>((intptr_t)reports.size());
  for (const auto &[_ts, report] : reports) {
    const int64 ts = _ts;
    QNetworkReply *response =
        ApiUtil::send(HttpMethod::Post, RHDC_API_HOST "/pl/crash",
                      "application/x-pl-crash-report; version=2",
                      QByteArray::fromStdString(report), false);
    ApiUtil::onResponse(
        response,
        [ts, pendingCount, response]() {
          DataProvider::setCrashLogSentDate(ts, Time::now());
          response->abort();
          pendingCount->fetch_sub(1);
        },
        [ts, pendingCount, response](ApiErrorType err) {
          if (err != ApiErrorType::NotFound &&
              err != ApiErrorType::ServerDown && err != ApiErrorType::Offline) {
            DataProvider::setCrashLogSentDate(ts, Time::now());
          }
          response->abort();
          pendingCount->fetch_sub(1);
        });
  }

  // Wait for up to 2 seconds before continuing in case a crash is happening
  // early and could interrupt the sending of the crash report(s)
  QTimer::singleShot(2000, [pendingCount]() { pendingCount->exchange(0); });

  while (pendingCount->load() > 0) {
    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents |
                                    QEventLoop::ExcludeUserInputEvents);
  }
}

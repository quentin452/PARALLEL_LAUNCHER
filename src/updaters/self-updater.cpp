#include "src/updaters/self-updater.hpp"

#ifndef __linux__

#include "src/core/file-controller.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/json.hpp"
#include "src/core/settings.hpp"
#include "src/core/version.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/process.hpp"
#include "src/ui/download-dialog.hpp"
#include "src/updaters/update-dialog.hpp"
#include <QCoreApplication>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <cstdlib>
#include <iostream>
#include <sstream>

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
#define ON_NETWORK_ERROR                                                       \
  QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred)
#else
#define ON_NETWORK_ERROR                                                       \
  QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error)
#endif

#ifdef __APPLE__
#define INSTALLER_URL                                                          \
  "https://parallel-launcher.ca/latest/parallel-launcher-x86_64.dmg"
#define INSTALLER_FILENAME "parallel-launcher-x86_64.dmg"
#else
#ifdef _WIN64
#define INSTALLER_URL                                                          \
  "https://parallel-launcher.ca/latest/parallel-launcher_setup_win64.exe"
#define INSTALLER_FILENAME "parallel-launcher_setup_win64.exe"
#else
#define INSTALLER_URL                                                          \
  "https://parallel-launcher.ca/latest/parallel-launcher_setup_win32.exe"
#define INSTALLER_FILENAME "parallel-launcher_setup_win32.exe"
#endif
#endif

static void showInstallError() {
  QMessageBox::critical(
      nullptr,
      QCoreApplication::translate("WindowsUpdater", "Unexpected Error"),
      QCoreApplication::translate("WindowsUpdater",
                                  "Failed to launch installer."));
}

static inline void downloadUpdate() {
  const fs::path filePath = BaseDir::temp() / INSTALLER_FILENAME;

  const DownloadResult status = DownloadDialog::download(
      QT_TRANSLATE_NOOP("DownloadDialog", "Downloading installer..."),
      INSTALLER_URL, filePath);
  if (!status.success) {
    QMessageBox::critical(
        nullptr,
        QCoreApplication::translate("WindowsUpdater", "Download Failed"),
        QCoreApplication::translate(
            "WindowsUpdater",
            "Failed to download the latest installer. Try again later.")
            .append('\n')
            .append(status.errorMessage.c_str()));
    return;
  }

#ifdef __APPLE__
  fs::path appPath = BaseDir::program();
  while (appPath.filename() != "parallel-launcher.app" &&
         appPath != appPath.root_directory()) {
    appPath = appPath.parent_path();
  }

  if (appPath == appPath.root_directory()) {
    showInstallError();
    return;
  }

  const fs::path tempMountPath = BaseDir::temp() / Uuid::Random().toString();
  const int exitCode =
      AsyncProcess("hdiutil", {"attach", filePath.u8string(), "-mountpoint",
                               tempMountPath.u8string()})
          .join();

  if (exitCode != 0) {
    showInstallError();
    return;
  }

  fs::error_code err;
  fs::remove_all(appPath, err);
  if (err) {
    AsyncProcess("hdiutil", {"detach", tempMountPath.u8string()}).join();
    showInstallError();
    return;
  }

  const bool updated =
      fs::tryCopyRecursive(tempMountPath / "parallel-launcher.app", appPath);

  AsyncProcess("hdiutil", {"detach", tempMountPath.u8string()}).join();
  if (!updated) {
    showInstallError();
    return;
  }

  AsyncProcess("open", {appPath.u8string()}).detach();
  std::exit(0);
#else
  try {
    AsyncProcess(filePath.u8string(), {"/SILENT", "/CLOSEAPPLICATIONS"})
        .detach();
    std::exit(0);
  } catch (...) {
    showInstallError();
  }
#endif
}

static inline void downloadChangelog(QNetworkAccessManager *web,
                                     const Version &latestVersion) {
  QNetworkReply *response = web->get(QNetworkRequest(
      QUrl("https://parallel-launcher.ca/latest/changelog.htm")));

  QObject::connect(response, ON_NETWORK_ERROR,
                   [=](QNetworkReply::NetworkError error) {
                     (void)error;
                     response->deleteLater();
                     web->deleteLater();
                   });

  QObject::connect(response, &QNetworkReply::finished, [=]() {
    if (response->error() != QNetworkReply::NoError)
      return;
    QByteArray rawData = response->readAll();
    web->deleteLater();
    response->deleteLater();

    UpdateDialog dialog(latestVersion, rawData.toStdString().c_str());
    const int result = dialog.exec();
    if (result == QDialog::Accepted) {
      downloadUpdate();
    } else if (dialog.stopReminders()) {
      AppSettings settings = FileController::loadAppSettings();
      settings.checkForUpdates = false;
      FileController::saveAppSettings(settings);
    }
  });
}

void SelfUpdater::checkForUpdates() {
  if (!FileController::loadAppSettings().checkForUpdates) {
    return;
  }

  QNetworkAccessManager *web = new QNetworkAccessManager();

  QNetworkReply *response = web->get(QNetworkRequest(
      QUrl("https://parallel-launcher.ca/latest/version.json")));

  QObject::connect(response, ON_NETWORK_ERROR,
                   [=](QNetworkReply::NetworkError error) {
                     (void)error;
                     response->deleteLater();
                     web->deleteLater();
                   });

  QObject::connect(response, &QNetworkReply::finished, [=]() {
    if (response->error() != QNetworkReply::NoError)
      return;

    Version latestVersion = {0, 0, 0};
    try {
      std::istringstream stream(response->readAll().data());
      const Json versionJson = Json::parse(stream);

#if defined(_WIN64)
      latestVersion = JsonSerializer::parse<Version>(versionJson["win_x64"]);
#elif defined(_WIN32)
			latestVersion = JsonSerializer::parse<Version>( versionJson["win_x86"] );
#else
			latestVersion = JsonSerializer::parse<Version>( versionJson["mac_x64"] );
#endif
    } catch (...) {
      std::cerr << "Failed to parse version info" << std::endl;
      response->deleteLater();
    }

    if (CurrentVersion::Application < latestVersion) {
      downloadChangelog(web, latestVersion);
    }
  });
}

#endif

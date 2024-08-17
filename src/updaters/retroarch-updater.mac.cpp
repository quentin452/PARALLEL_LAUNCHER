#ifdef __APPLE__
#include "src/updaters/retroarch-updater.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/qthread.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/updates.hpp"
#include "src/core/version.hpp"
#include "src/core/zip.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/ui/download-dialog.hpp"
#include <QCoreApplication>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <functional>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <thread>

struct MacVersionInfo {
  Version version;
  string zipSha1;
  string zipUrl;
};

static MacVersionInfo getLatestMacVersion() {
  QProgressDialog dialog;
  dialog.setRange(0, 0);
  dialog.setLabelText(QCoreApplication::translate(
      "WindowsRetroArchUpdater", "Checking for RetroArch Updates"));

  std::shared_ptr<bool> cancelled(new bool(false));
  MacVersionInfo versionInfo{Version{0, 0, 0}, "", ""};

  QNetworkAccessManager *web = new QNetworkAccessManager();
  QNetworkReply *response = web->get(QNetworkRequest(
      QUrl("https://parallel-launcher.ca/latest/retroarch.json")));

  QObject::connect(
      response, &QNetworkReply::finished, web,
      [web, response, cancelled, &versionInfo, &dialog]() {
        if (!*cancelled && response->error() == QNetworkReply::NoError) {
          try {
            std::istringstream stream(response->readAll().data());
            const Json json = Json::parse(stream);
            const Json &macVersion = json["macos_x64"];

            const Version version =
                JsonSerializer::parse<Version>(macVersion["version"]);
            const string sha1 = macVersion["zipSha1"].get<string>();
            const string url = macVersion["url"].get<string>();

            versionInfo.version = version;
            versionInfo.zipSha1 = sha1;
            versionInfo.zipUrl = url;
          } catch (...) {
          }
        }

        web->deleteLater();
        response->deleteLater();
        if (!*cancelled)
          dialog.close();
      },
      Qt::QueuedConnection);

  dialog.exec();
  *cancelled = true;

  return versionInfo;
}

static void installWorker(string expectedChecksum, QProgressDialog *dialog,
                          std::shared_ptr<bool> cancelled, bool *result) {
  const fs::path zipPath = BaseDir::temp() / "macos_retroarch.zip";
  bool success = (Sha1::compute(zipPath) == expectedChecksum);

  if (success) {
    success = Zip::unzip(zipPath, RetroArch::getExePath().parent_path());
  }

  if (success) {
    success =
        (chmod((RetroArch::getExePath() / "Contents" / "MacOS" / "RetroArch")
                   .c_str(),
               0755) == 0);
  }

  if (success) {
    const fs::path assetsZip =
        RetroArch::getExePath() / "Contents" / "Resources" / "assets.zip";
    const fs::path assetsDir = BaseDir::home() / "Library" /
                               "Application Support" / "RetroArch" / "assets";

    fs::error_code err;
    fs::remove_all(assetsDir, err);
    fs::create_directories(assetsDir, err);
    if (Zip::unzip(assetsZip, assetsDir)) {
      fs::remove(assetsZip, err);
    }
  }

  QtThread::safeAsync([=]() {
    if (*cancelled)
      return;
    *result = success;
    dialog->close();
  });
}

static bool downloadAndInstall(const MacVersionInfo &version) {
  fs::error_code err;

  const fs::path tempPath = BaseDir::temp() / "macos_retroarch.zip";
  fs::remove(tempPath, err);

  const DownloadResult downloadStatus = DownloadDialog::download(
      QT_TRANSLATE_NOOP("DownloadDialog", "Downloading RetroArch"),
      version.zipUrl, tempPath);

  if (!downloadStatus.success) {
    QMessageBox::critical(
        nullptr,
        QCoreApplication::translate("WindowsRetroArchUpdater",
                                    "Download Failed"),
        QCoreApplication::translate("WindowsRetroArchUpdater",
                                    "Failed to download RetroArch.")
            .append('\n')
            .append(downloadStatus.errorMessage.c_str()));
    return false;
  }

  QProgressDialog dialog;
  dialog.setRange(0, 0);
  dialog.setLabelText(QCoreApplication::translate("WindowsRetroArchUpdater",
                                                  "Installing RetroArch"));

  bool success = false;
  std::shared_ptr<bool> cancelled(new bool(false));

  fs::remove_all(RetroArch::getExePath(), err);
  std::thread(installWorker, version.zipSha1, &dialog, cancelled, &success)
      .detach();

  dialog.exec();
  *cancelled = true;

  fs::remove(tempPath, err);
  if (!success) {
    fs::remove_all(RetroArch::getExePath(), err);
    QMessageBox::critical(nullptr,
                          QCoreApplication::translate("WindowsRetroArchUpdater",
                                                      "Installation Error"),
                          QCoreApplication::translate(
                              "WindowsRetroArchUpdater",
                              "An error occurred attempting to uncompress the "
                              "portable RetroArch bundle"));
  }

  return success;
}

bool RetroArchUpdater::install() {
  const MacVersionInfo latest = getLatestMacVersion();
  if (latest.zipUrl.empty())
    return false;

  if (downloadAndInstall(latest)) {
    InstalledVersionsInfo versions = FileController::loadInstalledVersions();
    versions.retroArchVersion.version = latest.version;
    FileController::saveInstalledVersions(versions);
    return true;
  }

  return false;
}

void RetroArchUpdater::update() {
  InstalledVersionsInfo versions = FileController::loadInstalledVersions();

  const MacVersionInfo latest = getLatestMacVersion();
  if (versions.retroArchVersion.version >= latest.version)
    return;

  if (QMessageBox::question(
          nullptr,
          QCoreApplication::translate("WindowsRetroArchUpdater",
                                      "Install Update?"),
          QCoreApplication::translate("WindowsRetroArchUpdater",
                                      "An update for RetroArch is available. "
                                      "Would you like to install it now?")) !=
      QMessageBox::Yes) {
    return;
  }

  if (downloadAndInstall(latest)) {
    versions.retroArchVersion.version = latest.version;
    FileController::saveInstalledVersions(versions);
  }
}
#endif

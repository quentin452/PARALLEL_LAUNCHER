#include "src/ui/core-installer.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/logging.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/time.hpp"
#include "src/core/zip.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/crash.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/ui/download-dialog.hpp"
#include <QCoreApplication>
#include <QDateTime>
#include <QMessageBox>
#include <thread>

static bool installMupenCoreSync(const CoreBuild &coreBuild) {
  const fs::path tempPath = BaseDir::temp() / _NFS("core.zip");
  fs::create_directories(BaseDir::temp());

  const DownloadResult status = DownloadDialog::download(
      QT_TRANSLATE_NOOP("DownloadDialog", "Downloading core..."),
      coreBuild.downloadLink, tempPath);
  if (!status.success) {
    QMessageBox::critical(
        nullptr,
        QCoreApplication::translate("CoreInstaller", "Download Failed"),
        QCoreApplication::translate("CoreInstaller",
                                    "Failed to download emulator core")
            .append('\n')
            .append(status.errorMessage.c_str()));
    return false;
  }

  if (!Zip::unzip(tempPath, RetroArch::getCorePath())) {
    QMessageBox::critical(
        nullptr,
        QCoreApplication::translate("CoreInstaller", "Installation Failed"),
        QCoreApplication::translate("CoreInstaller",
                                    "Failed to install emulator core"));
    return false;
  }

  fs::error_code err;
  fs::remove(tempPath, err);

  QMessageBox::information(
      nullptr,
      QCoreApplication::translate("CoreInstaller", "Installation Successful"),
      QCoreApplication::translate("CoreInstaller",
                                  "Core installed successfully."));

  InstalledVersionsInfo versions = FileController::loadInstalledVersions();
  versions.mupenVersion.commit = coreBuild.commit.hash;
  FileController::saveInstalledVersions(versions);

  return true;
}

#if defined(_WIN32)
#define LIBRARY_EXT L".dll"
#elif defined(__APPLE__)
#define LIBRARY_EXT ".dylib"
#else
#define LIBRARY_EXT ".so"
#endif

static bool installParallelCoreSync(const ParallelCoreVersion &version) {
  const fs::path tempPath =
      BaseDir::temp() / _NFS("parallel_n64_next_libretro.zip");
  const fs::path corePath = RetroArch::getCorePath() /
                            (_NFS("parallel_n64_next_libretro") LIBRARY_EXT);
  const fs::path legacyCorePath =
      RetroArch::getCorePath() / (_NFS("parallel_n64_libretro") LIBRARY_EXT);

  fs::error_code err;
  fs::remove(tempPath, err);

  const DownloadResult status = DownloadDialog::download(
      QT_TRANSLATE_NOOP("DownloadDialog", "Downloading core..."),
      version.downloadUrl, tempPath);
  if (!status.success) {
    QMessageBox::critical(
        nullptr,
        QCoreApplication::translate("CoreInstaller", "Download Failed"),
        QCoreApplication::translate("CoreInstaller",
                                    "Failed to download emulator core")
            .append('\n')
            .append(status.errorMessage.c_str()));
    return false;
  }

  fs::remove(corePath, err);
  fs::remove(legacyCorePath, err);

  bool success = Zip::unzip(tempPath, RetroArch::getCorePath());
  if (!success)
    logError("Failed to unzip "s + tempPath.string() + " to " +
             RetroArch::getCorePath().string());

  if (success) {
    success = fs::existsSafe(corePath) || fs::existsSafe(legacyCorePath);
    if (!success)
      logError("Unzip was successful, but the expected file was not found.");
  }

  if (success && !fs::existsSafe(corePath)) {
    err.clear();
    fs::rename(legacyCorePath, corePath, err);
    success = !err;
  }

  if (success) {
    success = (Sha1::compute(corePath) == version.sha1);
    if (!success)
      logError("Checksum of downloaded core did not match the expected value");
  }

  if (!success) {
    fs::remove(corePath, err);
    fs::remove(legacyCorePath, err);
    QMessageBox::critical(
        nullptr,
        QCoreApplication::translate("CoreInstaller", "Installation Failed"),
        QCoreApplication::translate("CoreInstaller",
                                    "Failed to install emulator core"));
    return false;
  }

  InstalledVersionsInfo versions = FileController::loadInstalledVersions();
  versions.parallelVersion = version.version;
  FileController::saveInstalledVersions(versions);

  QMessageBox::information(
      nullptr,
      QCoreApplication::translate("CoreInstaller", "Installation Successful"),
      QCoreApplication::translate("CoreInstaller",
                                  "Core installed successfully."));

  return true;
}

static void
checkForMupenUpdates(const InstalledVersionsInfo &currentVersionInfo,
                     std::function<void(void)> then) {
#ifdef __APPLE__
  then();
  return;
#else
  const string currentCommit = currentVersionInfo.mupenVersion.commit;

  if (!RetroArch::isEmulatorCoreInstalled(EmulatorCore::Mupen64plusNext)) {
    then();
    return;
  }

  if (currentVersionInfo.mupenVersion.lock) {
    then();
    return;
  }

  MupenCoreBuilds::getLastKnownGood(
      FileController::loadAppSettings().mupenDevBranch ? "develop" : "master",
      currentCommit,
      [=](const CoreBuild &build) {
        if (QMessageBox::question(
                nullptr,
                QCoreApplication::translate("CoreInstaller",
                                            "Core Update Available"),
                QCoreApplication::translate("CoreInstaller",
                                            "An update is available for %1. "
                                            "Would you like to install it now?")
                    .arg("Mupen64Plus-Next")) == QMessageBox::Yes) {
          installMupenCoreSync(build);
        }
        then();
      },
      then);
#endif
}

static void
checkForParallelUpdates(const InstalledVersionsInfo &currentVersionInfo,
                        std::function<void(void)> then) {
  if (!RetroArch::isEmulatorCoreInstalled(EmulatorCore::ParallelN64)) {
    then();
    return;
  }

  ParallelCoreBuilds::getLatest(
      [=](const ParallelCoreVersion &latest) {
        const string currentHash =
            Sha1::compute(RetroArch::getCorePath() /
                          (_NFS("parallel_n64_next_libretro") LIBRARY_EXT));
        if (currentHash == latest.sha1 &&
            currentVersionInfo.parallelVersion == latest.version) {
          then();
          return;
        }

        if (QMessageBox::question(
                nullptr,
                QCoreApplication::translate("CoreInstaller",
                                            "Core Update Available"),
                QCoreApplication::translate("CoreInstaller",
                                            "An update is available for %1. "
                                            "Would you like to install it now?")
                    .arg("ParallelN64-Next")) == QMessageBox::Yes) {
          installParallelCoreSync(latest);
        }

        then();
      },
      then);
}

void CoreInstaller::checkForUpdatesAsync() {
  const InstalledVersionsInfo versions =
      FileController::loadInstalledVersions();
  checkForParallelUpdates(versions, [=]() {
    QMetaObject::invokeMethod(
        QCoreApplication::instance(),
        [=]() {
          checkForMupenUpdates(versions, [=]() {
            InstalledVersionsInfo updatedVersions =
                FileController::loadInstalledVersions();
            updatedVersions.lastUpdateCheck = Time::now();
            FileController::saveInstalledVersions(updatedVersions);
          });
        },
        Qt::QueuedConnection);
  });
}

void CoreInstaller::checkForUpdatesSync(InstalledVersionsInfo &versions) {
  bool done = false;
  checkForParallelUpdates(versions, [&]() {
    QMetaObject::invokeMethod(
        QCoreApplication::instance(),
        [&]() {
          checkForMupenUpdates(versions, [&]() {
            InstalledVersionsInfo updatedVersions =
                FileController::loadInstalledVersions();
            versions.mupenVersion = updatedVersions.mupenVersion;
            versions.parallelVersion = updatedVersions.parallelVersion;
            versions.lastUpdateCheck = Time::now();
            FileController::saveInstalledVersions(versions);
            done = true;
          });
        },
        Qt::QueuedConnection);
  });

  while (!done) {
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    std::this_thread::yield();
  }
}

#undef LIBRARY_EXT

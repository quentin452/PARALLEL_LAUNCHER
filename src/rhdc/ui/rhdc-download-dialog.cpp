#include "src/rhdc/ui/rhdc-download-dialog.hpp"
#include <ui_rhdc-download-dialog.h>

#include "src/core/bps.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/qthread.hpp"
#include "src/core/rom.hpp"
#include "src/core/sm64.hpp"
#include "src/core/zip.hpp"
#include "src/db/data-provider.hpp"
#include "src/db/transaction.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/rhdc/core/plugin-detect.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/toast.hpp"
#include <QTimer>
#include <QUrl>
#include <cctype>
#include <src/polyfill/base-directory.hpp>
#include <thread>

#ifdef _WIN32
#include "src/core/retroarch.hpp"
#include "src/core/save-converter.hpp"
#endif

RhdcDownloadDialog::RhdcDownloadDialog()
    : QDialog(nullptr), m_ui(new Ui::RhdcDownloadDialog) {
  m_ui->setupUi(this);
  setModal(true);
  setWindowIcon(Icon::appIcon());
  m_ui->progressBar->setRange(0, 0);
  m_ui->progressLabel->setVisible(false);
}

RhdcDownloadDialog::~RhdcDownloadDialog() { delete m_ui; }

static inline string getExtensionLowerCase(const fs::path &path) {
  string extension = path.extension().u8string();
  for (char &c : extension) {
    c = (char)std::tolower(c);
  }
  return extension;
}

static inline QString romProgressText(qint64 d, qint64 t) {
  char buffer[32];
  const double dm = (double)d / 1048576.0;
  const double dt = (double)t / 1048576.0;
  std::snprintf(buffer, 32, "%6.2f / %04.2f MiB", dm, dt);
  return QString(buffer);
}

static inline QString layoutProgressText(qint64 d, qint64 t) {
  char buffer[40];
  const double dm = (double)d / 1024.0;
  const double dt = (double)t / 1024.0;
  std::snprintf(buffer, 40, "%6.2f / %04.2f KiB", dm, dt);
  return QString(buffer);
}

fs::path RhdcDownloadDialog::run(const string &hackId,
                                 const string &downloadUrl) {
  return RhdcDownloadDialog::run(
      hackId, downloadUrl, QUrl(downloadUrl.c_str()).fileName().toStdString());
}

fs::path RhdcDownloadDialog::run(const string &hackId,
                                 const string &downloadUrl,
                                 const string &versionName) {
  RhdcDownloadDialog dialog;
  std::shared_ptr<bool> dialogExists = std::shared_ptr<bool>(new bool(true));

  RhdcApi::getStarpowerAsync(
      hackId,
      [=, &dialog](Starpower &starpower) {
        if (!*dialogExists)
          return;
        DataProvider::suggestRhdcHackPlayTime(hackId, starpower.playTime);
        dialog.getRecommendedPlugin(hackId, downloadUrl, versionName,
                                    dialogExists);
      },
      [=, &dialog](ApiErrorType) {
        if (!*dialogExists)
          return;
        dialog.getRecommendedPlugin(hackId, downloadUrl, versionName,
                                    dialogExists);
      });

  dialog.exec();
  *dialogExists = false;

  const string name =
      fs::to_path(QUrl(downloadUrl.c_str()).fileName().toStdString())
          .stem()
          .u8string();
  return FileController::loadRhdcSettings().downloadDirectory /
         fs::to_path(name + ' ' + hackId + ".z64");
}

static inline string lowercase(const string &str) {
  string lowerStr = "";
  for (char c : str) {
    lowerStr += (char)std::tolower(c);
  }
  return lowerStr;
}

static inline string urlDecode(const string &str) {
  string decoded = "";
  for (size_t i = 0; i < str.length();) {
    if (str[i] == '%' && i + 2 < str.length() && std::isxdigit(str[i + 1]) &&
        std::isxdigit(str[i + 2]) &&
        !(str[i + 1] == '0' && str[i + 2] == '0')) {
      decoded += (char)std::stoi(str.substr(i + 1, 2), nullptr, 16);
      i += 3;
    } else {
      decoded += str[i++];
    }
  }
  return decoded;
}

fs::path RhdcDownloadDialog::runForRhdcUrl(const string &rhdcUrl) {
  const string rhdcPath = rhdcUrl.substr(7);
  const size_t di = rhdcPath.find('/');

  if (di == string::npos)
    return fs::path();

  const string romSlug = rhdcPath.substr(0, di);
  string patch = lowercase(urlDecode(rhdcPath.substr(di + 1)));

  if (romSlug.empty() || patch.empty())
    return fs::path();

  RhdcDownloadDialog dialog;
  std::shared_ptr<bool> dialogExists = std::shared_ptr<bool>(new bool(true));
  fs::path downloadPath;
  string hackSha1;

  RhdcApi::getHackAsync(
      romSlug,
      [=, &downloadPath, &dialog, &hackSha1](RhdcVersionedHack &hack) {
        if (!*dialogExists)
          return;

        downloadPath.clear();
        if (hack.versions.empty()) {
          QTimer::singleShot(100, []() {
            ToastMessageManager::display(
                ToastType::Error, tr("Failed to download patch from RHDC"));
          });
          logWarn("Failed to launch from RHDC URL: hack has no versions");
          dialog.close();
          return;
        }

        DataProvider::addOrUpdateRhdcHack(hack.info);
        for (const string &author : hack.authors) {
          DataProvider::addRhdcHackAuthor(hack.info.hackId, author);
        }

        DataProvider::clearRhdcHackVersions(hack.info.hackId);
        int vi = 0;
        for (const RhdcHackVersionExt &v : hack.versions) {
          DataProvider::addRhdcHackVersion(
              hack.info.hackId, v.sha1, v.name, vi++,
              v.plugin.value_or(GfxPlugin::UseDefault), v.hackFlags,
              v.archived);
        }

        const RhdcHackVersionExt *version = &hack.versions.at(0);
        for (const RhdcHackVersionExt &v : hack.versions) {
          const string vname = lowercase(v.name);

          if (patch == vname) {
            version = &v;
          }

          if ((patch == "any" || patch == vname) &&
              DataProvider::tryFetchRomPathWithSha1(v.sha1, downloadPath)) {
            if (!hack.layoutUrl.empty()) {
              const int64 layoutDownloaded = RomUtil::getLastModified(
                  StarLayout::layoutPath(hack.info.hackId));
              if (layoutDownloaded < hack.layoutLastModified) {
                dialog.downloadLayout(
                    hack.info.hackId, ""s,
                    {version->plugin, version->pluginFlags, hack.layoutUrl},
                    dialogExists);
                return;
              }
            }

            dialog.close();
            return;
          }
        }

        downloadPath.clear();
        const string downloadUrl =
            RhdcApi::getDownloadUrl(hack.info.slug, version->name);
        DataProvider::addRomInfo(RomInfo{
            version->sha1,
            hack.info.name,
            "",
            EmulatorCore::UseDefault, // emulator and plugin will be set later
            GfxPlugin::UseDefault,
            GfxPlugin::UseDefault,
            false,
            true,
            false,
            false,
            false,
            0,
            0,
            std::set<string>(),
            !Flags::has(version->hackFlags, RhdcHackFlag::NoOverclock),
            false,
            0,
            Flags::has(version->hackFlags, RhdcHackFlag::DualAnalog)
                ? DefaultInputModes::DualAnalog.id
                : DefaultInputModes::Normal.id,
            WidescreenMode::Off,
            Flags::has(version->hackFlags, RhdcHackFlag::BigEEPROM),
            false,
            UNCHECKED_INPUT_TYPES,
            Flags::has(version->hackFlags, RhdcHackFlag::SupportsSD) ? "?"
                                                                     : ""});

        hackSha1 = version->sha1;
        dialog.downloadLayout(
            hack.info.hackId, downloadUrl,
            {version->plugin, version->pluginFlags, hack.layoutUrl},
            dialogExists);
      },
      [&dialog](ApiErrorType) {
        QTimer::singleShot(100, []() {
          ToastMessageManager::display(
              ToastType::Error, tr("Failed to download patch from RHDC"));
        });
        logWarn("Failed to launch from RHDC URL");
        dialog.close();
      });

  dialog.exec();
  *dialogExists = false;

  if (downloadPath.empty() && !hackSha1.empty()) {
    if (!DataProvider::tryFetchRomPathWithSha1(hackSha1, downloadPath)) {
      downloadPath.clear();
    }
  }

  if (!downloadPath.empty() && !fs::existsSafe(downloadPath)) {
    downloadPath.clear();
  }

  return downloadPath;
}

void RhdcDownloadDialog::getRecommendedPlugin(
    const string &hackId, const string &downloadUrl, const string &versionName,
    std::shared_ptr<bool> dialogExists) {
  if (!*dialogExists)
    return;
  RhdcApi::getLayoutAndRecommendedPluginAsync(
      hackId, versionName,
      [=](PluginAndLayoutInfo &rec) {
        this->downloadLayout(hackId, downloadUrl, rec, dialogExists);
      },
      [=](ApiErrorType) {
        this->downloadLayout(
            hackId, downloadUrl,
            PluginAndLayoutInfo{std::nullopt, PluginFlags::None, ""},
            dialogExists);
      });
}

void RhdcDownloadDialog::downloadLayout(const string &hackId,
                                        const string &hackDownloadUrl,
                                        const PluginAndLayoutInfo &hackInfo,
                                        std::shared_ptr<bool> dialogExists) {
  if (!*dialogExists)
    return;

  if (hackInfo.layoutUrl.empty()) {
    if (hackDownloadUrl.empty()) {
      close();
    } else {
      downloadHack(hackId, hackDownloadUrl, hackInfo, dialogExists);
    }
    return;
  }

  fs::error_code err;
  const string layoutUrl = hackInfo.layoutUrl;
  const fs::path savePath = StarLayout::layoutPath(hackId);
  fs::create_directories(savePath.parent_path(), err);

  m_ui->statusLabel->setText(tr("Downloading star layout"));
  m_ui->progressBar->setRange(0, 100);
  m_ui->progressBar->setValue(0);

  QFile *file = new QFile(savePath.u8string().c_str());
  file->open(QIODevice::WriteOnly | QIODevice::Truncate);
  RhdcApi::downloadFile(
      layoutUrl, file,
      [=](int64 downloaded, int64 total) {
        if (!*dialogExists)
          return;
        this->m_ui->progressLabel->setVisible(true);
        this->m_ui->progressLabel->setText(
            layoutProgressText(downloaded, total));
        this->m_ui->progressBar->setValue(
            (int)(100.0 * downloaded / total + 0.5));
      },
      [=]() {
        file->close();
        file->deleteLater();
        if (hackDownloadUrl.empty()) {
          this->close();
        } else {
          this->downloadHack(hackId, hackDownloadUrl, hackInfo, dialogExists);
        }
      },
      [=](ApiErrorType) {
        file->close();
        file->remove();
        file->deleteLater();

        ToastMessageManager::display(
            ToastType::Warn, tr("Failed to download star layout from RHDC"));
        logWarn("Failed to download star layout from RHDC: "s + layoutUrl);

        if (hackDownloadUrl.empty()) {
          this->close();
        } else {
          this->downloadHack(hackId, hackDownloadUrl, hackInfo, dialogExists);
        }
      });
}

void RhdcDownloadDialog::downloadHack(const string &hackId,
                                      const string &downloadUrl,
                                      const PluginAndLayoutInfo &hackInfo,
                                      std::shared_ptr<bool> dialogExists) {
  if (!*dialogExists)
    return;

  m_ui->statusLabel->setText(tr("Downloading patch"));
  m_ui->progressBar->setRange(0, 100);
  m_ui->progressBar->setValue(0);

  const QUrl url(downloadUrl.c_str());
  const fs::path tempDownloadPath =
      BaseDir::temp() / fs::to_path(url.fileName().toStdString());
  QFile *file = new QFile(tempDownloadPath.u8string().c_str());
  file->open(QIODevice::WriteOnly | QIODevice::Truncate);

  RhdcApi::downloadFile(
      url.toString().toStdString(), file,
      [=](int64 downloaded, int64 total) {
        if (!*dialogExists)
          return;
        this->m_ui->progressLabel->setVisible(true);
        this->m_ui->progressLabel->setText(romProgressText(downloaded, total));
        this->m_ui->progressBar->setValue(
            (int)(100.0 * downloaded / total + 0.5));
      },
      [=]() {
        file->close();
        if (*dialogExists) {
          DataProvider::deleteRomPath(downloadUrl);

          this->m_ui->statusLabel->setText(tr("Applying patch"));
          this->m_ui->progressBar->setRange(0, 0);
          this->m_ui->progressLabel->setVisible(false);
          this->m_ui->dialogButtonBox->setEnabled(false);

          fs::path sm64path;
          if (SM64::tryGetPath(sm64path)) {
            std::thread(RhdcDownloadDialog::patchRom, this, hackId,
                        tempDownloadPath, sm64path, hackInfo, dialogExists)
                .detach();
          } else {
            ToastMessageManager::display(
                ToastType::Error,
                tr("The rom could not be installed because you do not have an "
                   "unmodified copy of the US version of Super Mario 64 in "
                   "your known roms list."));
            logWarn("Could not patch RHDC rom because no vanilla SM64 base rom "
                    "was found");

            fs::error_code err;
            fs::remove(tempDownloadPath, err);
            this->close();
          }
        } else {
          file->remove();
        }
        file->deleteLater();
      },
      [=](ApiErrorType) {
        file->close();
        file->remove();
        file->deleteLater();

        ToastMessageManager::display(ToastType::Error,
                                     tr("Failed to download patch from RHDC"));
        logWarn("Failed to download patch from RHDC: "s + downloadUrl);

        if (*dialogExists)
          this->close();
      });
}

void RhdcDownloadDialog::patchRom(RhdcDownloadDialog *dialog, string hackId,
                                  fs::path patchPath, fs::path baseRomPath,
                                  PluginAndLayoutInfo hackInfo,
                                  std::shared_ptr<bool> dialogExists) {
  fs::error_code err;
  if (getExtensionLowerCase(patchPath) == ".zip") {
    const fs::path zipDir = BaseDir::temp() / _NFS("rhdc-extracted");
    fs::remove_all(zipDir, err);
    fs::create_directories(zipDir, err);
    Zip::unzip(patchPath, zipDir);
    bool foundPatch = false;
    for (const auto &i : fs::recursive_directory_iterator(zipDir)) {
      if (fs::isDirectorySafe(i.path()))
        continue;
      if (getExtensionLowerCase(i.path()) != ".bps")
        continue;
      patchPath = patchPath.parent_path() / i.path().filename();
      fs::rename(i.path(), patchPath, err);
      if (fs::isRegularFileSafe(patchPath)) {
        foundPatch = true;
        break;
      }
    }
    fs::remove_all(zipDir, err);
    if (!foundPatch)
      patchPath = fs::path();
  }

  if (getExtensionLowerCase(patchPath) != ".bps") {
    QtThread::safeAsync([=]() {
      ToastMessageManager::display(
          ToastType::Error,
          tr("The rom could not be installed because no bps patch was found."));
      logWarn("Could not install rom because no bps patch was found");
      if (*dialogExists)
        dialog->close();
    });
    return;
  }

  const string romFilename =
      patchPath.stem().u8string() + ' ' + hackId + ".z64";
  fs::path romPath = FileController::loadRhdcSettings().downloadDirectory /
                     fs::to_path(romFilename);

  const Bps::BpsApplyError bpsErr =
      Bps::tryApplyBps(patchPath, baseRomPath, romPath);
  if (bpsErr != Bps::BpsApplyError::None) {
    fs::remove(patchPath, err);
    QtThread::safeAsync([=]() {
      ToastMessageManager::display(
          ToastType::Error, tr("Could not install '%1' because of an "
                               "unexpected error while applying the patch")
                                .arg(patchPath.stem().u8string().c_str()));
      logWarn("Could not install '"s + patchPath.stem().u8string() +
              "' because of an unexpected error while applying the patch");
      if (*dialogExists)
        dialog->close();
    });
    return;
  }

  fs::remove(patchPath, err);
  QtThread::safeAsync([=]() {
    if (!*dialogExists)
      return;
    dialog->m_ui->statusLabel->setText(tr("Computing checksum"));
  });

  const string sha1 = Sha1::compute(romPath);
  QtThread::safeAsync([=]() {
    const AutoPlugin autoPlugin =
        AutoPlugin::detect(romPath, hackInfo.plugin, hackInfo.pluginFlags);
    const string internalName = RomUtil::getInternalName(romPath);

    SqlTransaction transaction;
    DataProvider::removeOnlineRom(hackId);
    DataProvider::addRomFile(
        {romPath, RomUtil::getLastModified(romPath), sha1, true});
    DataProvider::updatePluginSettings(
        sha1, autoPlugin.emulator, autoPlugin.parallelPlugin,
        autoPlugin.mupenPlugin, autoPlugin.upscaleTexrects, true,
        autoPlugin.emulateFramebuffer, autoPlugin.accurateDepthCompare,
        autoPlugin.emulateRsp);
    DataProvider::updateRomHeaderInfo(sha1, internalName,
                                      RomUtil::getCrc32(romPath),
                                      RomUtil::readControllerTypes(romPath));
    transaction.commit();

#ifdef _WIN32
    SaveConverter::autoImportFromProject64(RetroArch::getSaveFilePath(romPath),
                                           internalName);
#endif

    ToastMessageManager::display(ToastType::Info,
                                 tr("Romhack installed successfully"));

    if (*dialogExists)
      dialog->close();
  });
}

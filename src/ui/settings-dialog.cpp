#include "src/ui/settings-dialog.hpp"
#include "ui_settings-dialog.h"

#include "src/core/file-controller.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/steam-deck.hpp"
#include "src/core/updates.hpp"
#include "src/db/data-provider.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file-dialog.hpp"
#include "src/rhdc/core/sync.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include <QMessageBox>

#if defined(__linux__) && defined(_LP64)
#include "src/core/discord.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/ui/download-dialog.hpp"
#endif

#ifdef _WIN32
#include "src/polyfill/windows/locale.hpp"
#else
#include <QLocale>
#endif

#ifndef __linux__
#include <QStyleFactory>
#endif

#ifdef __APPLE__
#include "src/polyfill/macos/apple-util.hpp"
#endif

static const GfxPlugin m_parallelPlugins[] = {
    GfxPlugin::ParaLLEl, GfxPlugin::Glide64, GfxPlugin::Angrylion,
    GfxPlugin::Rice,     GfxPlugin::Gln64,   GfxPlugin::GLideN64,
    GfxPlugin::OGRE};

static const GfxPlugin m_mupenPlugins[] = {
    GfxPlugin::ParaLLEl, GfxPlugin::Angrylion, GfxPlugin::GLideN64};

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent), m_ui(new Ui::SettingsDialog) {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());
  UiUtil::scaleWindow(this);
  UiUtil::fixDialogButtonsOnWindows(this);

  m_ui->bpsBrowseButton->setIcon(Icon::browse());
  m_ui->rhdcBrowseButton->setIcon(Icon::browse());
  m_ui->resetConfigButton->setIcon(Icon::undo());
  m_ui->applyAll1->setIcon(Icon::saveAll());
  m_ui->applyAll2->setIcon(Icon::saveAll());
  m_ui->applyAll3->setIcon(Icon::saveAll());
  m_ui->applyAll4->setIcon(Icon::saveAll());

  UiUtil::setIconSize(m_ui->applyAll1, 16);
  UiUtil::setIconSize(m_ui->applyAll2, 16);
  UiUtil::setIconSize(m_ui->applyAll3, 16);
  UiUtil::setIconSize(m_ui->applyAll4, 16);

#ifdef _WIN32
  const QString userLocale = WindowsLocale::getUserLocale().c_str();
#else
  const QString userLocale = QLocale::system().name();
#endif

  m_ui->languageSelect->addItem(tr("Automatic (%1)").arg(userLocale),
                                QVariant::fromValue<QString>(""));
  m_ui->languageSelect->addItem("English (Canadian)",
                                QVariant::fromValue<QString>("en_CA"));
  m_ui->languageSelect->addItem("English (American)",
                                QVariant::fromValue<QString>("en_US"));
  m_ui->languageSelect->addItem("English (British)",
                                QVariant::fromValue<QString>("en_GB"));
  m_ui->languageSelect->addItem("Español", QVariant::fromValue<QString>("es"));
  m_ui->languageSelect->addItem("Français", QVariant::fromValue<QString>("fr"));
  m_ui->languageSelect->addItem("Français (Québécois)",
                                QVariant::fromValue<QString>("fr_CA"));
  m_ui->languageSelect->addItem("Deutsch", QVariant::fromValue<QString>("de"));
  m_ui->languageSelect->addItem("Italiano", QVariant::fromValue<QString>("it"));
  m_ui->languageSelect->addItem("اَلْعَرَبِيَّةُ", QVariant::fromValue<QString>("ar"));
  m_ui->languageSelect->addItem("Português",
                                QVariant::fromValue<QString>("pt"));

#ifndef __linux__
  m_ui->themeSelect->addItems(QStyleFactory::keys());
#else
  m_ui->themePanel->setVisible(false);
  m_ui->darkModeCheckbox->setVisible(false);
#endif

#ifdef __linux__
  m_ui->updateCheckbox->setVisible(false);
#ifndef _LP64
  m_ui->discordIntegrationCheckbox->setVisible(false);
#endif
#endif

#if defined(__APPLE__)
  m_ui->forceX86Checkbox->setVisible(AppleUtil::isSilicon());
#else
  m_ui->forceX86Checkbox->setVisible(false);
#endif

  if (SteamDeck::isSteamDeck()) {
    m_ui->label->setVisible(false);
    m_ui->windowScaleSelect->setVisible(false);
    m_ui->fullscreenCheckbox->setVisible(false);
    m_ui->preferHleCheckbox->setVisible(false);
    m_ui->hideLauncherCheckbox->setVisible(false);
  }
}

SettingsDialog::~SettingsDialog() { delete m_ui; }

void SettingsDialog::showEvent(QShowEvent *event) {
  const AppSettings settings = FileController::loadAppSettings();
  const InstalledVersionsInfo versions =
      FileController::loadInstalledVersions();

#ifdef _WIN32
  m_ui->audioDriverSelect->setCurrentIndex((int)settings.audioDriver);
#else
  m_ui->audioDriverLabel->setVisible(false);
  m_ui->audioDriverSelect->setVisible(false);
#endif

  m_ui->windowScaleSelect->setCurrentIndex((int)settings.windowScale - 1);
  switch (settings.defaultParallelPlugin) {
  case GfxPlugin::Glide64:
    m_ui->parallelPluginSelect->setCurrentIndex(1);
    break;
  case GfxPlugin::Angrylion:
    m_ui->parallelPluginSelect->setCurrentIndex(2);
    break;
  case GfxPlugin::Rice:
    m_ui->parallelPluginSelect->setCurrentIndex(3);
    break;
  case GfxPlugin::Gln64:
    m_ui->parallelPluginSelect->setCurrentIndex(4);
    break;
  case GfxPlugin::GLideN64:
    m_ui->parallelPluginSelect->setCurrentIndex(5);
    break;
  case GfxPlugin::OGRE:
    m_ui->parallelPluginSelect->setCurrentIndex(6);
    break;
  default:
    m_ui->parallelPluginSelect->setCurrentIndex(0);
    break;
  }
  switch (settings.defaultMupenPlugin) {
  case GfxPlugin::GLideN64:
    m_ui->mupenPluginSelect->setCurrentIndex(2);
    break;
  case GfxPlugin::Angrylion:
    m_ui->mupenPluginSelect->setCurrentIndex(1);
    break;
  default:
    m_ui->mupenPluginSelect->setCurrentIndex(0);
    break;
  }
  m_ui->emulatorCoreSelect->setCurrentIndex((int)settings.defaultEmulator - 1);
  m_ui->fullscreenCheckbox->setChecked(settings.fullscreen);
  m_ui->vsyncCheckbox->setChecked(settings.vsync);
  m_ui->pauseOnBlurCheckbox->setChecked(settings.pauseOnBlur);
  m_ui->hideNotificationsCheckbox->setChecked(
      settings.hideRetroArchNotifications);
  m_ui->hideLauncherCheckbox->setChecked(settings.hideWhenPlaying);
  m_ui->showInternalNameCheckbox->setChecked(
      Flags::has(settings.visibleColumns, RomInfoColumn::InternalName));
  m_ui->showFilePathCheckbox->setChecked(
      Flags::has(settings.visibleColumns, RomInfoColumn::Path));
  m_ui->showLastPlayedCheckbox->setChecked(
      Flags::has(settings.visibleColumns, RomInfoColumn::LastPlayed));
  m_ui->showPlayTimeCheckbox->setChecked(
      Flags::has(settings.visibleColumns, RomInfoColumn::PlayTime));
  m_ui->discordIntegrationCheckbox->setChecked(settings.discordIntegration);
  m_ui->advancedSettingsCheckbox->setChecked(settings.showAdvancedSettings);
  m_ui->emulateSdCardCheckbox->setChecked(settings.emulateSdCard);
#ifndef __linux__
  m_ui->updateCheckbox->setChecked(settings.checkForUpdates);
  m_ui->themeSelect->setCurrentText(settings.theme.c_str());
  m_ui->darkModeCheckbox->setChecked(settings.darkMode);
#endif

  m_ui->languageSelect->setCurrentIndex(0);
  for (int i = 0; i < m_ui->languageSelect->count(); i++) {
    if (settings.locale == m_ui->languageSelect->itemData(i, Qt::UserRole)
                               .toString()
                               .toStdString()) {
      m_ui->languageSelect->setCurrentIndex(i);
      break;
    }
  }

  m_ui->bpsRelativeRadio->setChecked(settings.patchToSameFolder);
  m_ui->bpsFixedRadio->setChecked(!settings.patchToSameFolder);
  m_ui->bpsSavePathPanel->setEnabled(!settings.patchToSameFolder);
  m_ui->bpsPath->setText(settings.patchedRomFolder.u8string().c_str());

  m_ui->upscalingSelect->setCurrentIndex((int)settings.parallelUpscaling);
  m_ui->parallelAntiAliasingCheckbox->setChecked(settings.parallelAntiAliasing);
  m_ui->upscaleTexRectsCheckbox->setChecked(settings.parallelTexRectUpscaling);
  m_ui->removeBordersCheckbox->setChecked(settings.parallelRemoveBorders);
  m_ui->angrylionFilteringSelect->setCurrentIndex(
      (int)settings.angrylionFiltering);
  m_ui->glidenAntiAliasingCheckbox->setChecked(settings.glidenAntiAliasing);
  m_ui->glidenPointFilteringCheckbox->setChecked(
      settings.glidenThreePointFiltering);
  m_ui->glidenDepthBufferCheckbox->setChecked(
      settings.glidenFramebufferEmulation &&
      settings.glidenCorrectDepthCompare);
  m_ui->emulateFramebufferCheckbox->setChecked(
      settings.glidenFramebufferEmulation);
  m_ui->glidenDepthBufferCheckbox->setEnabled(
      settings.glidenFramebufferEmulation);

  m_ui->updateRetroarchCheckbox->setChecked(!versions.retroArchVersion.lock);
  m_ui->updateMupenCheckbox->setChecked(!versions.mupenVersion.lock);
  m_ui->useDevBranchCheckbox->setChecked(settings.mupenDevBranch);
  m_ui->updateIntervalSelect->setCurrentIndex((int)settings.coreUpdateInterval);

  const RhdcSettings rhdcSettings = FileController::loadRhdcSettings();
  m_ui->rhdcPathInput->setText(
      rhdcSettings.downloadDirectory.u8string().c_str());
  m_ui->enableStarDisplayCheckbox->setChecked(rhdcSettings.enableStarDisplay);
  m_ui->allSlotsCheckbox->setChecked(rhdcSettings.checkAllSaveSlots);
  m_ui->preferHleCheckbox->setChecked(rhdcSettings.preferHle);
  m_ui->enableDefaultLayoutsCheckbox->setChecked(
      rhdcSettings.enableDefaultLayouts);
  m_ui->starDisplaySubsection->setEnabled(rhdcSettings.enableStarDisplay);
  m_ui->ignoreWidescreenCheckbox->setChecked(rhdcSettings.ignoreWidescreenHint);

#ifdef __APPLE__
  m_ui->glidenAntiAliasingCheckbox->setChecked(false);
  m_ui->glidenAntiAliasingCheckbox->setEnabled(false);
  m_ui->forceX86Checkbox->setChecked(settings.forceX86);
#endif

  m_ui->enableStarDisplayCheckbox->setVisible(!SteamDeck::inGamingMode());
  m_ui->starDisplaySubsection->setVisible(!SteamDeck::inGamingMode());

  m_ui->isViewerCheckbox->setChecked(settings.enableIsViewer);
  m_ui->historySizeLabel->setEnabled(settings.enableIsViewer);
  m_ui->historySizeSpinner->setEnabled(settings.enableIsViewer);
  m_ui->historySizeSpinner->setValue(
      settings.isViewerHistorySize ? settings.isViewerHistorySize : 1050);

  m_ui->systemClockRadio->setChecked(settings.rtcOffset == 0);
  m_ui->manualTimeRadio->setChecked(settings.rtcOffset != 0);
  m_ui->rtcInput->setEnabled(settings.rtcOffset != 0);
  m_ui->rtcInput->setOffset(settings.rtcOffset);
  m_ui->rtcSavestateCheckbox->setChecked(settings.rtcSavestates);

  windowScaleChanged((int)settings.windowScale - 1);

  QDialog::showEvent(event);
  UiUtil::shrinkToFit(this, Qt::Horizontal | Qt::Vertical);
}

void SettingsDialog::closeEvent(QCloseEvent *event) {
  save();
  QDialog::closeEvent(event);
}

void SettingsDialog::save() {
  AppSettings settings = FileController::loadAppSettings();
  InstalledVersionsInfo versions = FileController::loadInstalledVersions();

  settings.visibleColumns = (RomInfoColumn)0;
  if (m_ui->showInternalNameCheckbox->isChecked())
    settings.visibleColumns |= RomInfoColumn::InternalName;
  if (m_ui->showFilePathCheckbox->isChecked())
    settings.visibleColumns |= RomInfoColumn::Path;
  if (m_ui->showLastPlayedCheckbox->isChecked())
    settings.visibleColumns |= RomInfoColumn::LastPlayed;
  if (m_ui->showPlayTimeCheckbox->isChecked())
    settings.visibleColumns |= RomInfoColumn::PlayTime;

#ifndef __linux__
  settings.theme = m_ui->themeSelect->currentText().toStdString();
  settings.darkMode = m_ui->darkModeCheckbox->isChecked();
#endif
  settings.locale =
      m_ui->languageSelect->currentData(Qt::UserRole).toString().toStdString();

  if (settings.parallelTexRectUpscaling !=
      m_ui->upscaleTexRectsCheckbox->isChecked()) {
    DataProvider::setDefaultParallelTexRectUpscaling(
        m_ui->upscaleTexRectsCheckbox->isChecked());
  }

  if (settings.glidenFramebufferEmulation !=
      m_ui->emulateFramebufferCheckbox->isChecked()) {
    DataProvider::setDefaultGlidenFrameBufferEmulation(
        m_ui->emulateFramebufferCheckbox->isChecked());
  }

  if (settings.glidenCorrectDepthCompare !=
      m_ui->glidenDepthBufferCheckbox->isChecked()) {
    DataProvider::setDefaultGlidenAccurateDepthCompare(
        m_ui->glidenDepthBufferCheckbox->isChecked());
  }

  settings.defaultParallelPlugin =
      m_parallelPlugins[m_ui->parallelPluginSelect->currentIndex()];
  settings.defaultMupenPlugin =
      m_mupenPlugins[m_ui->mupenPluginSelect->currentIndex()];
  settings.defaultEmulator =
      (EmulatorCore)(m_ui->emulatorCoreSelect->currentIndex() + 1);
  settings.windowScale = (ubyte)(m_ui->windowScaleSelect->currentIndex() + 1);
  settings.fullscreen = m_ui->fullscreenCheckbox->isChecked();
  settings.vsync = m_ui->vsyncCheckbox->isChecked();
  settings.pauseOnBlur = m_ui->pauseOnBlurCheckbox->isChecked();
  settings.hideRetroArchNotifications =
      m_ui->hideNotificationsCheckbox->isChecked();
  settings.emulateSdCard = m_ui->emulateSdCardCheckbox->isChecked();
  settings.hideWhenPlaying = m_ui->hideLauncherCheckbox->isChecked();
  settings.patchToSameFolder = m_ui->bpsRelativeRadio->isChecked();
  settings.patchedRomFolder = fs::to_path(m_ui->bpsPath->text().toStdString());
#if defined(__linux__) && !defined(_LP64)
  settings.discordIntegration = false;
#else
  settings.discordIntegration = m_ui->discordIntegrationCheckbox->isChecked();
#endif
  settings.showAdvancedSettings = m_ui->advancedSettingsCheckbox->isChecked();
#ifdef _WIN32
  settings.audioDriver = (AudioDriver)m_ui->audioDriverSelect->currentIndex();
#endif

#ifndef __linux__
  settings.checkForUpdates = m_ui->updateCheckbox->isChecked();
#endif

  settings.parallelUpscaling =
      (ParallelUpscaling)m_ui->upscalingSelect->currentIndex();
  settings.parallelAntiAliasing =
      m_ui->parallelAntiAliasingCheckbox->isChecked();
  settings.parallelTexRectUpscaling =
      m_ui->upscaleTexRectsCheckbox->isChecked();
  settings.parallelRemoveBorders = m_ui->removeBordersCheckbox->isChecked();
  settings.angrylionFiltering =
      (AngrylionFiltering)m_ui->angrylionFilteringSelect->currentIndex();
  settings.glidenAntiAliasing = m_ui->glidenAntiAliasingCheckbox->isChecked();
  settings.glidenThreePointFiltering =
      m_ui->glidenPointFilteringCheckbox->isChecked();
  settings.glidenFramebufferEmulation =
      m_ui->emulateFramebufferCheckbox->isChecked();
  if (m_ui->glidenDepthBufferCheckbox->isEnabled()) {
    settings.glidenCorrectDepthCompare =
        m_ui->glidenDepthBufferCheckbox->isChecked();
  }

  versions.retroArchVersion.lock = !m_ui->updateRetroarchCheckbox->isChecked();
  versions.mupenVersion.lock = !m_ui->updateMupenCheckbox->isChecked();
  settings.mupenDevBranch = m_ui->useDevBranchCheckbox->isChecked();
  settings.coreUpdateInterval =
      (CoreUpdateInterval)m_ui->updateIntervalSelect->currentIndex();

#if defined(__APPLE__)
  settings.forceX86 = m_ui->forceX86Checkbox->isChecked();
#endif

  RhdcSettings rhdcSettings = FileController::loadRhdcSettings();
  const fs::path newRhdcDownloadDir =
      fs::to_path(m_ui->rhdcPathInput->text().toStdString());
  if (rhdcSettings.downloadDirectory != newRhdcDownloadDir) {
    RHDC::moveRhdcFolder(rhdcSettings.downloadDirectory, newRhdcDownloadDir);
    rhdcSettings.downloadDirectory = newRhdcDownloadDir;
  }
  rhdcSettings.enableStarDisplay = m_ui->enableStarDisplayCheckbox->isChecked();
  rhdcSettings.checkAllSaveSlots = m_ui->allSlotsCheckbox->isChecked();
  rhdcSettings.preferHle = m_ui->preferHleCheckbox->isChecked();
  rhdcSettings.enableDefaultLayouts =
      m_ui->enableDefaultLayoutsCheckbox->isChecked();
  rhdcSettings.ignoreWidescreenHint =
      m_ui->ignoreWidescreenCheckbox->isChecked();

#if defined(__linux__) && defined(_LP64) && defined(DISCORD_LAZY_LINKING)
  if (settings.discordIntegration && !Discord::pluginInstalled()) {
    if (QMessageBox::question(
            this, tr("Install Discord Plugin?"),
            tr("You have enabled Discord integration, but the Discord plugin "
               "is not currently installed. Would you like to install it "
               "now?")) == QMessageBox::Yes) {
      fs::error_code err;
      fs::create_directories(BaseDir::data() / "plugins", err);
      const DownloadResult result = DownloadDialog::download(
          QT_TRANSLATE_NOOP("DownloadDialog", "Downloading Discord plugin"),
          "https://parallel-launcher.ca/optional/linux_64/libdiscord-rpc.so",
          BaseDir::data() / "plugins" / "libdiscord-rpc.so");

      if (!result.success) {
        QMessageBox::critical(this, tr("Download Failed"),
                              tr("Failed to download Discord plugin")
                                  .append("\n")
                                  .append(result.errorMessage.c_str()));
        settings.discordIntegration = false;
      }
    } else {
      settings.discordIntegration = false;
    }
  }
#endif

  settings.enableIsViewer = m_ui->isViewerCheckbox->isChecked();
  settings.isViewerHistorySize = (m_ui->historySizeSpinner->value() <= 1000)
                                     ? m_ui->historySizeSpinner->value()
                                     : 0;

  settings.rtcOffset =
      m_ui->manualTimeRadio->isChecked() ? m_ui->rtcInput->offset() : 0;
  settings.rtcSavestates = m_ui->rtcSavestateCheckbox->isChecked();

  FileController::saveAppSettings(settings);
  FileController::saveInstalledVersions(versions);
  FileController::saveRhdcSettings(rhdcSettings);
}

void SettingsDialog::windowScaleChanged(int index) {
  int autoScale;
  switch (index) {
  case 0:
    autoScale = 1;
    break;
  case 1:
    autoScale = 2;
    break;
  case 7:
    autoScale = 8;
    break;
  case 8:
    autoScale = 9;
    break;
  default:
    autoScale = 4;
    break;
  }

  const int autoWidth = 320 * autoScale;
  const int autoHeight = 240 * autoScale;

  const string autoRes = " (x" + Number::toString(autoScale) + " - " +
                         Number::toString(autoWidth) + 'x' +
                         Number::toString(autoHeight) + ')';
  m_ui->upscalingSelect->setItemText(0, tr("Auto") + QString(autoRes.c_str()));
}

void SettingsDialog::browseForBpsPath() {
  fs::path folderPath =
      FileDialog::getDirectory(tr("Select a Folder").toStdString().c_str());
  if (!folderPath.empty()) {
    m_ui->bpsPath->setText(folderPath.u8string().c_str());
  }
}

void SettingsDialog::browseForRhdcDir() {
  fs::path folderPath =
      FileDialog::getDirectory(tr("Select a Folder").toStdString().c_str());
  if (!folderPath.empty()) {
    m_ui->rhdcPathInput->setText(folderPath.u8string().c_str());
  }
}

void SettingsDialog::resetConfig() {
  if (QMessageBox::question(
          this, tr("Confirm Reset"),
          tr("This will reset your RetroArch config file, undoing any changes "
             "you have made within RetroArch. Your Parallel Launcher settings "
             "will not be affected. Do you want to continue?")) ==
      QMessageBox::Yes) {
    if (RetroArch::resetConfig()) {
      const fs::path systemDir =
          BaseDir::data() / _NFS("retro-data") / _NFS("system");
      fs::error_code err;
      fs::remove_all(systemDir / _NFS("ParallelN64") / _NFS("shaders"), err);
      fs::remove_all(systemDir / _NFS("Mupen64plus") / _NFS("shaders"), err);
      QMessageBox::information(this, tr("Config Reset"),
                               tr("Your RetroArch config has been reset."));
    } else {
      QMessageBox::critical(this, tr("Oops"),
                            tr("An unknown error occurred. Your RetroArch "
                               "config has not been reset."));
    }
  }
}

void SettingsDialog::updateCores() {
  AppSettings settings = FileController::loadAppSettings();
  InstalledVersionsInfo coreVersions = FileController::loadInstalledVersions();
  coreVersions.retroArchVersion.lock =
      !m_ui->updateRetroarchCheckbox->isChecked();
  coreVersions.mupenVersion.lock = !m_ui->updateMupenCheckbox->isChecked();
  settings.mupenDevBranch = m_ui->useDevBranchCheckbox->isChecked();
  FileController::saveAppSettings(settings);
  FileController::saveInstalledVersions(coreVersions);
#ifndef FLATPAK_VERSION
  RetroUpdater::checkForUpdates(false, true);
#endif
}

void SettingsDialog::applyAll_accurateDepthCompare() {
  if (QMessageBox::question(this, tr("Confirm Apply All"),
                            tr("Apply this setting to all current roms?")) ==
      QMessageBox::Yes) {
    DataProvider::setAllGlidenAccurateDepthCompare(
        m_ui->glidenDepthBufferCheckbox->isChecked());
  }
}

void SettingsDialog::applyAll_emulateFramebuffer() {
  if (QMessageBox::question(this, tr("Confirm Apply All"),
                            tr("Apply this setting to all current roms?")) ==
      QMessageBox::Yes) {
    DataProvider::setAllGlidenFrameBufferEmulation(
        m_ui->emulateFramebufferCheckbox->isChecked());
  }
}

void SettingsDialog::applyAll_upscaleTexRects() {
  if (QMessageBox::question(this, tr("Confirm Apply All"),
                            tr("Apply this setting to all current roms?")) ==
      QMessageBox::Yes) {
    DataProvider::setAllParallelTexRectUpscaling(
        m_ui->upscaleTexRectsCheckbox->isChecked());
  }
}

void SettingsDialog::applyAll_removeBorders() {
  if (QMessageBox::question(this, tr("Confirm Apply All"),
                            tr("Apply this setting to all current roms?")) ==
      QMessageBox::Yes) {
    DataProvider::setDefaultParallelRemoveBorders(
        m_ui->removeBordersCheckbox->isChecked());
  }
}

void SettingsDialog::fbEmulationToggled(bool enabled) {
  m_ui->glidenDepthBufferCheckbox->setEnabled(enabled);
  m_ui->glidenDepthBufferCheckbox->setChecked(
      enabled ? FileController::loadAppSettings().glidenCorrectDepthCompare
              : false);
}

void SettingsDialog::historySizeChanged(int value) {
  if (value > 1000) {
    m_ui->historySizeSpinner->setSpecialValueText(tr("Unlimited"));
  } else {
    m_ui->historySizeSpinner->setSpecialValueText(QString());
  }
}

void SettingsDialog::rtcSyncToggled(bool syncToClock) {
  m_ui->rtcInput->setEnabled(!syncToClock);
  if (syncToClock)
    m_ui->rtcInput->setOffset(0);
}

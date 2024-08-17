#include "src/ui/main-window.hpp"
#include "ui_main-window.h"

#include "src/core/file-controller.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/rom-finder.hpp"
#include "src/core/rom.hpp"
#include "src/core/special-groups.hpp"
#include "src/core/time.hpp"
#include "src/core/version.hpp"
#include "src/db/data-provider.hpp"
#include "src/input/gamepad-controller.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file-dialog.hpp"
#include "src/polyfill/file-explorer.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/rhdc/core/credentials.hpp"
#include "src/rhdc/core/sync.hpp"
#include "src/rhdc/ui/rhdc-download-dialog.hpp"
#include "src/rhdc/ui/rhdc-login-dialog.hpp"
#include "src/rhdc/ui/rhdc-view-bubble.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/ui/controller-select-dialog.hpp"
#include "src/ui/core-installer.hpp"
#include "src/ui/crash-report-dialog.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/keyboard-config-dialog.hpp"
#include "src/ui/log-viewer-dialog.hpp"
#include "src/ui/now-playing-window.hpp"
#include "src/ui/play.hpp"
#include "src/ui/rom-list-model.hpp"
#include "src/ui/rom-source-dialog.hpp"
#include "src/ui/save-file-editor-dialog.hpp"
#include "src/ui/save-slot-editor-dialog.hpp"
#include "src/ui/settings-dialog.hpp"
#include "src/ui/toast.hpp"
#include "src/ui/util.hpp"
#include <QCheckBox>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>

static constexpr int VIEW_NO_ROMS = 0;
static constexpr int VIEW_CLASSIC = 1;
static constexpr int VIEW_RHDC = 2;

MainWindow::MainWindow()
    : QMainWindow(nullptr), m_ui(new Ui::MainWindow), m_showAllPlugins(false),
      m_windowSizingComplete(false),
      m_settings(FileController::loadAppSettings()), m_exists(new bool(true)),
      m_romSettingsWidget(new RomSettingsWidget(this)) {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());
  ToastMessageManager::setWindow(this);

  connect(m_romSettingsWidget, &RomSettingsWidget::rspEmulationChanged, this,
          &MainWindow::rspEmulationChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::accurateDepthCompareChanged,
          this, &MainWindow::accurateDepthCompareChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::emulateFramebufferChanged,
          this, &MainWindow::emulateFramebufferChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::emulatorCoreChanged, this,
          &MainWindow::emulatorChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::inputModeChanged, this,
          &MainWindow::inputModeChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::mupenPluginChanged, this,
          &MainWindow::mupenPluginChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::overclockCpuChanged, this,
          &MainWindow::overclockCpuChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::overclockViChanged, this,
          &MainWindow::overclockViChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::parallelPluginChanged, this,
          &MainWindow::parallelPluginChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::showMorePluginsChanged, this,
          &MainWindow::showMorePluginsChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::upscaleTexRectsChanged, this,
          &MainWindow::upscaleTexRectsChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::removeBordersChanged, this,
          &MainWindow::removeBordersChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::widescreenChanged, this,
          &MainWindow::widescreenChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::bigEepromChanged, this,
          &MainWindow::bigEepromChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::useInterpreterChanged, this,
          &MainWindow::useInterpreterChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::saveSyncingChanged, this,
          &MainWindow::saveSyncingChanged);
  connect(m_romSettingsWidget, &RomSettingsWidget::activeSdCardChanged, this,
          &MainWindow::sdCardChanged);

#ifdef __APPLE__
  UiUtil::fixFontSizeOnMac(m_ui->titleLabel);
  if (m_settings.theme == "macintosh") {
    const char *styleFix = R"#(
min-height: 41px;
padding-left: 8px;
padding-right: 8px;
margin-bottom: 2px;
)#";
    m_ui->rhdcViewToggleButton->setStyleSheet(styleFix);
    m_ui->refreshButton->setStyleSheet(styleFix);
    m_ui->controllerConfigButton->setStyleSheet(styleFix);
    m_ui->rhdcViewToggleButton->setStyleSheet(styleFix);

    m_ui->centralwidget->layout()->setSpacing(0);
    m_ui->horizontalLayout->layout()->setSpacing(6);
    m_ui->romView->layout()->setContentsMargins(0, 0, 0, 0);
    m_ui->romListPage->layout()->setContentsMargins(0, 0, 0, 0);
    m_ui->rhdcPage->layout()->setContentsMargins(0, 0, 0, 0);
    m_ui->rhdcPage->layout()->setSpacing(0);
  }
#endif

  m_ui->titleLabel->setText(
      QString(("Parallel Launcher v"s + CurrentVersion::Application.toString())
                  .c_str()));

  if (palette().buttonText().color().valueF() > 0.5) {
    m_ui->rhdcViewToggleButton->setIcon(Icon::rhdcLight());
  } else {
    m_ui->rhdcViewToggleButton->setIcon(Icon::rhdcDark());
  }

  m_ui->controllerConfigButton->setIcon(Icon::gamepad());
  m_ui->menuButton->setIcon(Icon::menu());
  m_ui->refreshButton->setIcon(Icon::refresh());
  m_ui->playSingleplayerButton->setIcon(Icon::play());
  m_ui->playMultiplayerButton->setIcon(Icon::group());
  m_ui->downloadButton->setIcon(Icon::download());

  UiUtil::setIconSize(m_ui->rhdcViewToggleButton, 24);
  UiUtil::setIconSize(m_ui->controllerConfigButton, 24);
  UiUtil::setIconSize(m_ui->refreshButton, 24);
  UiUtil::setIconSize(m_ui->menuButton, 24);

  const UiState uiState = FileController::loadUiState();
  m_ui->romList->refetchAll();
  m_ui->romList->restoreTreeState(uiState.romList);
  setView(m_ui->romList->hasRoms() ? VIEW_CLASSIC : VIEW_NO_ROMS, true);

  m_ui->actionSettings->setIcon(Icon::configure());
  m_ui->actionManageSources->setIcon(Icon::search());
  m_ui->actionAddRom->setIcon(Icon::add());
  m_ui->actionConfigureControllers->setIcon(Icon::gamepad());
  m_ui->actionConfigureKeyboard->setIcon(Icon::keyboard());
  m_ui->rhdcLoginAction->setIcon(Icon::login());
  m_ui->rhdcLogoutAction->setIcon(Icon::logout());
  m_ui->rhdcDisableAction->setIcon(Icon::cancel());
  m_ui->actionQuit->setIcon(Icon::logout());

  QMenu *settingsMenu = new QMenu(m_ui->menuButton);
  QMenu *directoryMenu = new QMenu(settingsMenu);
  directoryMenu->setTitle(tr("Data Directories"));
  directoryMenu->setIcon(Icon::browse());
  m_ui->menuButton->setMenu(settingsMenu);

  settingsMenu->addAction(m_ui->actionSettings);
  settingsMenu->addMenu(directoryMenu);
  settingsMenu->addAction(m_ui->actionManageSources);
  settingsMenu->addAction(m_ui->actionAddRom);
  settingsMenu->addAction(m_ui->actionConfigureControllers);
  settingsMenu->addAction(m_ui->actionConfigureKeyboard);
  settingsMenu->addAction(m_ui->rhdcLoginAction);
  settingsMenu->addAction(m_ui->rhdcLogoutAction);
  settingsMenu->addAction(m_ui->rhdcDisableAction);
  settingsMenu->addAction(m_ui->actionQuit);

  directoryMenu->addAction(m_ui->actionOpenSaveDir);
  directoryMenu->addAction(m_ui->actionOpenSavestateDir);
  directoryMenu->addAction(m_ui->actionOpenSdCardDir);
  directoryMenu->addSeparator();
  directoryMenu->addAction(m_ui->actionOpenDataDir);
  directoryMenu->addAction(m_ui->actionOpenConfigDir);
  directoryMenu->addAction(m_ui->actionOpenCacheDir);

  if (uiState.rhdcView) {
    resize(uiState.rhdcViewSize.width, uiState.rhdcViewSize.height);
  } else {
    resize(uiState.classicViewSize.width, uiState.classicViewSize.height);
  }

  m_ui->searchBar->addAction(Icon::search(), QLineEdit::LeadingPosition);

  m_showAllPlugins = uiState.showAllPlugins;
  m_romSettingsWidget->setShowAllPlugins(m_showAllPlugins);

  connect(m_ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshRomList()));
  connect(m_ui->romList->selectionModel(),
          SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
          this, SLOT(romSelectionChanged()));
  connect(m_ui->romList, SIGNAL(selectionFilteredOut()), this,
          SLOT(romSelectionChanged()));
  connect(m_ui->romList, SIGNAL(editSave(fs::path)), this,
          SLOT(editSave(fs::path)));
  connect(m_ui->rhdcPage, SIGNAL(editSave(fs::path)), this,
          SLOT(editSave(fs::path)));
  connect(m_ui->romList, SIGNAL(launchRom()), this, SLOT(playSingleplayer()));
  connect(m_ui->romList, SIGNAL(downloadPatch()), this, SLOT(download()));
  connect(m_ui->rhdcPage, SIGNAL(romSelected()), this,
          SLOT(romSelectionChanged()));

  reloadSettings();
  romSelectionChanged();

  refreshRomList();
  updateRhdcActions();

  if (uiState.rhdcView) {
    setView(VIEW_RHDC, true);
    m_ui->rhdcViewToggleButton->setIcon(Icon::appIcon());
    m_ui->rhdcViewToggleButton->setVisible(true);
  }

  if (!FileController::loadAppSettings().knowsAboutRhdcView &&
      !uiState.rhdcView) {
    new RhdcViewBubble(this, m_ui->rhdcViewToggleButton);
  }
}

MainWindow::~MainWindow() {
  NowPlayingWindow::free();
  m_ui->romList->model()->deleteLater();
  delete m_ui;
}

void MainWindow::setView(int view, bool keepSize) {
  const QSizePolicy preferred =
      QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  const QSizePolicy ignored =
      QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  const bool rhdcView = (view == VIEW_RHDC);
  const bool wasRhdcView = (m_ui->romView->currentIndex() == VIEW_RHDC);

  QSize newSize;
  if (!keepSize && rhdcView != wasRhdcView) {
    if (rhdcView) {
      UiState uiState = FileController::loadUiState();
      uiState.classicViewSize.width = width();
      uiState.classicViewSize.height = height();
      FileController::saveUiState(uiState);
      newSize = QSize(uiState.rhdcViewSize.width, uiState.rhdcViewSize.height);
    } else {
      UiState uiState = FileController::loadUiState();
      uiState.rhdcViewSize.width = width();
      uiState.rhdcViewSize.height = height();
      FileController::saveUiState(uiState);
      newSize =
          QSize(uiState.classicViewSize.width, uiState.classicViewSize.height);
    }
  }

  if (rhdcView) {
    m_ui->rhdcPage->settingsSlot()->place(m_romSettingsWidget);
  } else {
    m_ui->settingsSlot->place(m_romSettingsWidget);
  }

  m_ui->noRomSourcesPages->setSizePolicy(rhdcView ? ignored : preferred);
  m_ui->romListPage->setSizePolicy(rhdcView ? ignored : preferred);
  m_ui->rhdcPage->setSizePolicy(rhdcView ? preferred : ignored);

  m_ui->romView->setCurrentIndex(view);
  if (newSize.isValid())
    resize(newSize);
}

RomInfo *MainWindow::getSelectedRomInfo() {
  switch (m_ui->romView->currentIndex()) {
  case VIEW_CLASSIC:
    return m_ui->romList->tryGetSelectedRom().info;
  case VIEW_RHDC:
    return m_ui->romList->tryGetRomInfo(m_ui->rhdcPage->getSelectedVersion());
  default:
    return nullptr;
  }
}

void MainWindow::reloadSettings() {
  m_settings = FileController::loadAppSettings();

  m_ui->romList->setColumnHidden(0, false);
  m_ui->romList->setColumnHidden(
      1, !Flags::has(m_settings.visibleColumns, RomInfoColumn::InternalName));
  m_ui->romList->setColumnHidden(
      2, !Flags::has(m_settings.visibleColumns, RomInfoColumn::Path));
  m_ui->romList->setColumnHidden(
      3, !Flags::has(m_settings.visibleColumns, RomInfoColumn::LastPlayed));
  m_ui->romList->setColumnHidden(
      4, !Flags::has(m_settings.visibleColumns, RomInfoColumn::PlayTime));
}

void MainWindow::refreshRomList() {
  m_romSearch.cancel();
  m_ui->searchIndicator->setVisible(true);
  std::shared_ptr<bool> windowExists = m_exists;
  m_romSearch = RomFinder::scanAsync(
      [this, windowExists](RomFinder::SearchResults result) {
        if (!*windowExists || result == RomFinder::SearchResults::Cancelled)
          return;
        if (result == RomFinder::SearchResults::Updated) {
          this->refetchAll();
        }

        RHDC::sync([this, windowExists](bool refetch) {
          this->m_ui->searchIndicator->setVisible(false);
          updateRhdcActions();
          if (refetch)
            this->refetchAll();
        });
      });
}

void MainWindow::romSelectionChanged() {
  if (m_ui->romView->currentIndex() == VIEW_RHDC) {
    const string sha1 = m_ui->rhdcPage->getSelectedVersion();

    RomInfo romInfo;
    if (sha1.empty() ||
        !DataProvider::tryFetchRomByHash(sha1, false, &romInfo)) {
      m_romSettingsWidget->setEnabled(false);
      m_ui->downloadButton->setVisible(false);
      m_ui->playSingleplayerButton->setVisible(false);
      m_ui->playMultiplayerButton->setVisible(false);
      return;
    }

    if (romInfo.inputTypes == UNCHECKED_INPUT_TYPES) {
      fs::path filePath;
      if (DataProvider::tryFetchRomPathWithSha1(sha1, filePath)) {
        romInfo.inputTypes = RomUtil::readControllerTypes(filePath);
        DataProvider::updateRomHeaderInfo(sha1, romInfo.internalName,
                                          romInfo.crc32, romInfo.inputTypes);
      }
    }

    const string hackId = m_ui->rhdcPage->getSelectedHackId();
    const RhdcSettingHints hints = DataProvider::getRhdcHints(sha1);
    m_romSettingsWidget->setEnabled(true);
    m_romSettingsWidget->setSettings(
        romInfo.inputModeId, romInfo.emulator, romInfo.parallelPlugin,
        romInfo.mupenPlugin, romInfo.overclockCPU, romInfo.overclockVI,
        romInfo.widescreen, romInfo.bigEEPROM, romInfo.useInterpreter,
        romInfo.parallelTexRectUpscaling, romInfo.parallelRemoveBorders,
        romInfo.glidenFramebufferEmulation, romInfo.glidenCorrectDepthCompare,
        romInfo.glidenParallelRsp, hints.recommendedPlugin,
        !Flags::has(hints.hackFlags, RhdcHackFlag::NoOverclock),
        romInfo.inputTypes,
        hackId.empty()
            ? std::nullopt
            : std::optional<bool>(DataProvider::groupSaveEnabled(hackId)),
        (Flags::has(hints.hackFlags, RhdcHackFlag::SupportsSD) &&
         romInfo.sdCard.empty())
            ? "?"s
            : romInfo.sdCard);

    const bool isDownloaded = DataProvider::romWithSha1Exists(sha1);
    m_ui->downloadButton->setVisible(!isDownloaded);
    m_ui->playSingleplayerButton->setVisible(isDownloaded);
    m_ui->playMultiplayerButton->setVisible(isDownloaded);
  } else {
    RomReference rom = m_ui->romList->tryGetSelectedRom();
    const bool romSelected = (rom.info != nullptr);

    m_romSettingsWidget->setEnabled(romSelected);
    if (romSelected) {
      if (rom.info->inputTypes == UNCHECKED_INPUT_TYPES &&
          rom.file != nullptr && rom.file->local) {
        rom.info->inputTypes = RomUtil::readControllerTypes(rom.file->path);
        m_ui->romList->updateRomInfo(*rom.info, false, false);

        DataProvider::updateRomHeaderInfo(
            rom.info->sha1, rom.info->internalName, rom.info->crc32,
            rom.info->inputTypes);
      }

      const RhdcSettingHints hints = DataProvider::getRhdcHints(rom.info->sha1);
      m_romSettingsWidget->setSettings(
          rom.info->inputModeId, rom.info->emulator, rom.info->parallelPlugin,
          rom.info->mupenPlugin, rom.info->overclockCPU, rom.info->overclockVI,
          rom.info->widescreen, rom.info->bigEEPROM, rom.info->useInterpreter,
          rom.info->parallelTexRectUpscaling, rom.info->parallelRemoveBorders,
          rom.info->glidenFramebufferEmulation,
          rom.info->glidenCorrectDepthCompare, rom.info->glidenParallelRsp,
          hints.recommendedPlugin,
          !Flags::has(hints.hackFlags, RhdcHackFlag::NoOverclock),
          rom.info->inputTypes,
          (rom.rhdc != nullptr)
              ? std::optional<bool>(
                    DataProvider::groupSaveEnabled(rom.rhdc->info.hackId))
              : std::nullopt,
          (Flags::has(hints.hackFlags, RhdcHackFlag::SupportsSD) &&
           rom.info->sdCard.empty())
              ? "?"s
              : rom.info->sdCard);
    }

    const bool showDownloadButton = rom.file != nullptr && !rom.file->local;
    m_ui->downloadButton->setVisible(showDownloadButton);
    m_ui->playSingleplayerButton->setVisible(!showDownloadButton &&
                                             rom.file != nullptr);
    m_ui->playMultiplayerButton->setVisible(!showDownloadButton &&
                                            rom.file != nullptr);
  }
}

void MainWindow::overclockCpuChanged(bool overclock) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->overclockCPU = overclock;
  DataProvider::updateRomCpuOverclocking(romInfo->sha1, overclock);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::overclockViChanged(bool overclock) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->overclockVI = overclock;
  DataProvider::updateRomViOverclocking(romInfo->sha1, overclock);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::widescreenChanged(ubyte wide) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->widescreen = (WidescreenMode)wide;
  DataProvider::updateRomWidescreen(romInfo->sha1, (WidescreenMode)wide);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::bigEepromChanged(bool bigEeprom) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->bigEEPROM = bigEeprom;
  DataProvider::updateRomBigEeprom(romInfo->sha1, bigEeprom);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::useInterpreterChanged(bool useInterpreter) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->useInterpreter = useInterpreter;
  DataProvider::updateRomUseInterpreter(romInfo->sha1, useInterpreter);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::parallelPluginChanged() {
  const GfxPlugin plugin = m_romSettingsWidget->getParallelPlugin();

  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr || romInfo->parallelPlugin == plugin)
    return;

  romInfo->parallelPlugin = plugin;
  DataProvider::updateRomParallelPlugin(romInfo->sha1, plugin);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::mupenPluginChanged() {
  const GfxPlugin plugin = m_romSettingsWidget->getMupenPlugin();

  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr || romInfo->mupenPlugin == plugin)
    return;

  romInfo->mupenPlugin = plugin;
  DataProvider::updateRomMupenPlugin(romInfo->sha1, plugin);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::inputModeChanged() {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->inputModeId = m_romSettingsWidget->getInputMode();
  DataProvider::updateRomInputMode(romInfo->sha1, romInfo->inputModeId);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::emulatorChanged() {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->emulator = m_romSettingsWidget->getEmulatorCore();
  DataProvider::updateRomEmulator(romInfo->sha1, romInfo->emulator);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::upscaleTexRectsChanged(bool upscale) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->parallelTexRectUpscaling = upscale;
  DataProvider::setRomParallelTexRectUpscaling(romInfo->sha1, upscale);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::removeBordersChanged(bool removeBorders) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->parallelRemoveBorders = removeBorders;
  DataProvider::setRomParallelRemoveBorders(romInfo->sha1, removeBorders);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::emulateFramebufferChanged(bool emulate) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->glidenFramebufferEmulation = emulate;
  DataProvider::setRomGlidenFrameBufferEmulation(romInfo->sha1, emulate);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::accurateDepthCompareChanged(bool accurate) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->glidenCorrectDepthCompare = accurate;
  DataProvider::setRomGlidenAccurateDepthCompare(romInfo->sha1, accurate);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::rspEmulationChanged(bool emulate) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  romInfo->glidenParallelRsp = emulate;
  DataProvider::setRomGlidenRspEmulation(romInfo->sha1, emulate);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::sdCardChanged(std::string sdCard) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  if (sdCard.empty() && romInfo->sdCard == "?")
    return;
  if (sdCard == romInfo->sdCard)
    return;

  romInfo->sdCard = sdCard;
  DataProvider::updateSdCard(romInfo->sha1, sdCard);
  m_ui->romList->updateRomInfo(*romInfo, false, false);
}

void MainWindow::showMorePluginsChanged(bool showAll) {
  m_showAllPlugins = showAll;
}

void MainWindow::saveSyncingChanged(bool sync) {
  RomInfo *romInfo = getSelectedRomInfo();
  if (romInfo == nullptr)
    return;

  string hackId;
  if (DataProvider::tryFetchHackIdByChecksum(romInfo->sha1, hackId)) {
    if (sync) {
      DataProvider::enableGroupSave(hackId);
    } else {
      DataProvider::disableGroupSave(hackId);
    }
  }
}

void MainWindow::play(bool multiplayer) {
  RomInfo romInfo;
  RomFile romFile;

  if (m_ui->romView->currentIndex() == VIEW_RHDC) {
    if (!DataProvider::tryFetchRomByHash(m_ui->rhdcPage->getSelectedVersion(),
                                         false, &romInfo) ||
        !DataProvider::tryFetchRomFileWithSha1(
            romInfo.sha1, m_ui->rhdcPage->getCurrentPlaylist(), &romFile))
      return;
  } else {
    const RomReference rom = m_ui->romList->tryGetSelectedRom();
    if (rom.info == nullptr)
      return;

    romInfo = *rom.info;
    romFile = *rom.file;
  }

  if (!romFile.local)
    return;

  const QRect winGeo = geometry();
  hide();

  Game::play(romFile, romInfo, multiplayer, false, [this, winGeo]() {
    this->setGeometry(winGeo);
    this->show();
    QTimer::singleShot(50, [this, winGeo]() { this->setGeometry(winGeo); });
    this->m_romSettingsWidget->refreshSdCards();
    this->refetchAll();
  });
}

void MainWindow::playSingleplayer() { play(false); }

void MainWindow::playMultiplayer() { play(true); }

void MainWindow::configureController() {
  ControllerSelectDialog dialog;
  dialog.exec();
}

void MainWindow::configureKeyboard() {
  KeyboardConfigDialog dialog;
  dialog.exec();
  m_romSettingsWidget->reloadHotkeyInfo();
}

void MainWindow::refetchAll() {
  m_ui->romList->refetchAll();
  if (m_ui->romView->currentIndex() == VIEW_RHDC) {
    m_ui->rhdcPage->reload();
  } else {
    setView(m_ui->romList->hasRoms() ? VIEW_CLASSIC : VIEW_NO_ROMS);
  }
}

void MainWindow::editSettings() {
  SettingsDialog *dialog = new SettingsDialog();
  dialog->exec();
  delete dialog;
  reloadSettings();
  refetchAll();
  romSelectionChanged();
}

void MainWindow::manageRomSources() {
  RomSourceDialog *dialog = new RomSourceDialog();
  dialog->exec();
  delete dialog;
  refetchAll();
  refreshRomList();
}

void MainWindow::addRom() {
  fs::path romPath =
      FileDialog::getFile(tr("Select a ROM").toStdString().c_str(),
                          {{tr("ROM or Patch File").toStdString().c_str(),
                            "*.z64;*.n64;*.v64;*.bps"}});
  if (romPath.empty())
    return;

  static const std::set<string> s_allowedExtensions = std::set<string>{
      ".z64", ".n64", ".v64", ".Z64", ".N64", ".V64", ".bps", ".BPS", ".Bps"};

  static const std::set<string> s_patchExtensions =
      std::set<string>{".bps", ".BPS", ".Bps"};

  if (s_allowedExtensions.count(romPath.extension().u8string()) <= 0) {
    return;
  }

  if (!RomUtil::coveredBySearchPath(romPath)) {
    DataProvider::addManualRomPath(romPath);
  }

  string firstGroup = SpecialGroups::Uncategorized;
  if (DataProvider::tryFetchRomFile(romPath, nullptr)) {
    RomInfo romInfo;
    if (DataProvider::tryFetchRomByPath(romPath, true, &romInfo, nullptr) &&
        !romInfo.groups.empty()) {
      firstGroup = *romInfo.groups.begin();
    }
  } else {
    const string sha1 = Sha1::compute(romPath);

    DataProvider::addRomFile(
        RomFile{romPath, RomUtil::getLastModified(romPath), sha1, true});

    const AppSettings &settings = FileController::loadAppSettings();
    DataProvider::addRomInfo(RomInfo{// does nothing if the sha1 already exists
                                     sha1,
                                     "",
                                     RomUtil::getInternalName(romPath),
                                     EmulatorCore::UseDefault,
                                     GfxPlugin::UseDefault,
                                     GfxPlugin::UseDefault,
                                     settings.parallelTexRectUpscaling,
                                     settings.parallelRemoveBorders,
                                     settings.glidenFramebufferEmulation,
                                     settings.glidenCorrectDepthCompare,
                                     false,
                                     0,
                                     0,
                                     std::set<string>(),
                                     true,
                                     false,
                                     RomUtil::getCrc32(romPath),
                                     DefaultInputModes::Normal.id,
                                     WidescreenMode::Off,
                                     false,
                                     false,
                                     RomUtil::readControllerTypes(romPath),
                                     ""});
  }

  setView(VIEW_CLASSIC);
  refetchAll();
  m_ui->romList->selectRom(SpecialGroups::Uncategorized, romPath);
}

void MainWindow::paintEvent(QPaintEvent *event) {
  QMainWindow::paintEvent(event);
  if (!m_windowSizingComplete) {
    const UiState uiState = FileController::loadUiState();
    if (m_ui->romView->currentIndex() == VIEW_RHDC) {
      resize(uiState.rhdcViewSize.width, uiState.rhdcViewSize.height);
    } else {
      resize(uiState.classicViewSize.width, uiState.classicViewSize.height);
    }
    m_windowSizingComplete = true;
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  const UiState &oldState = FileController::loadUiState();

  UiState uiState = {m_ui->romList->saveTreeState(),
                     m_ui->rhdcPage->uiState(),
                     oldState.classicViewSize,
                     oldState.rhdcViewSize,
                     m_showAllPlugins,
                     m_ui->romView->currentIndex() == VIEW_RHDC};

  if (uiState.rhdcView) {
    uiState.rhdcViewSize.width = width();
    uiState.rhdcViewSize.height = height();
  } else {
    uiState.classicViewSize.width = width();
    uiState.classicViewSize.height = height();
  }

  FileController::saveUiState(uiState);
  NowPlayingWindow::free();
  QMainWindow::closeEvent(event);
  QCoreApplication::quit();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_F7) {
    event->accept();
    LogViewerDialog().exec();
  } else if (event->key() == Qt::Key_F8) {
    event->accept();
    CrashReportDialog().exec();
  } else {
    QMainWindow::keyPressEvent(event);
  }
}

void MainWindow::editSave(fs::path saveFilePath) {
  SaveFileEditorDialog dialog(saveFilePath);
  if (dialog.exec() == QDialog::Accepted) {
    SaveSlotEditorDialog dialog2(saveFilePath, dialog.getSaveSlot());
    dialog2.exec();
  }
}

void MainWindow::updateRhdcActions() {
  const bool integrationEnabled =
      RhdcCredentials::exists() || DataProvider::hasRhdcData();
  const bool loggedIn = RhdcApi::isAuthenticated();

  m_ui->rhdcLoginAction->setText(integrationEnabled
                                     ? tr("Login to romhacking.com")
                                     : tr("Enable romhacking.com integration"));
  m_ui->rhdcLoginAction->setIcon(integrationEnabled ? Icon::login()
                                                    : Icon::plugin());
  m_ui->rhdcLoginAction->setVisible(!loggedIn);
  m_ui->rhdcLogoutAction->setVisible(loggedIn);
  m_ui->rhdcDisableAction->setVisible(integrationEnabled);
  m_ui->rhdcViewToggleButton->setVisible(integrationEnabled && loggedIn);

  if (m_ui->romView->currentIndex() == VIEW_RHDC &&
      (!integrationEnabled || !loggedIn)) {
    setView(m_ui->romList->hasRoms() ? VIEW_CLASSIC : VIEW_NO_ROMS);
    if (palette().buttonText().color().valueF() > 0.5) {
      m_ui->rhdcViewToggleButton->setIcon(Icon::rhdcLight());
    } else {
      m_ui->rhdcViewToggleButton->setIcon(Icon::rhdcDark());
    }
  }
}

void MainWindow::rhdcLogin() {
  RhdcLoginDialog dialog;
  if (dialog.exec() == QDialog::Accepted) {
    m_ui->searchIndicator->setVisible(true);
    updateRhdcActions();

    std::shared_ptr<bool> windowExists = m_exists;
    RHDC::sync([this, windowExists](bool refetch) {
      if (!*windowExists)
        return;
      this->m_ui->searchIndicator->setVisible(false);
      if (refetch)
        this->refetchAll();
    });
  }
}

void MainWindow::rhdcLogout() {
  RhdcApi::logout();
  RhdcCredentials::forget();
  updateRhdcActions();
}

void MainWindow::rhdcDisable() {
  if (QMessageBox::question(
          this, tr("Confirm Disable"),
          tr("Are you sure you want to disable romhacking.com integration?")) ==
      QMessageBox::Yes) {
    m_romSearch.cancel();
    m_ui->searchIndicator->setVisible(false);
    RhdcApi::logout();
    RhdcCredentials::forget();
    DataProvider::clearAllRhdcData();
    updateRhdcActions();
  }
}

void MainWindow::download() {
  string hackId, downloadUrl;
  if (m_ui->romView->currentIndex() == VIEW_RHDC) {
    const string sha1 = m_ui->rhdcPage->getSelectedVersion();
    if (DataProvider::romWithSha1Exists(sha1) ||
        !m_ui->rhdcPage->tryGetHackIdAndDownloadUrl(hackId, downloadUrl))
      return;
  } else {
    const RomReference rom = m_ui->romList->tryGetSelectedRom();
    if (rom.file == nullptr || rom.file->local || !rom.rhdc)
      return;
    hackId = rom.rhdc->info.hackId;
    downloadUrl = rom.file->path.u8string();
  }

  TreeUiState uiState = m_ui->romList->saveTreeState();
  uiState.selectedRom = RhdcDownloadDialog::run(hackId, downloadUrl);
  m_ui->romList->refetchAll();
  m_ui->romList->restoreTreeState(uiState);
  romSelectionChanged();
}

void MainWindow::rhdcViewToggled() {
  if (m_ui->romView->currentIndex() < VIEW_RHDC) {
    setView(VIEW_RHDC);
    m_ui->rhdcViewToggleButton->setIcon(Icon::appIcon());
  } else {
    m_ui->romList->refetchAll();
    setView(m_ui->romList->hasRoms() ? VIEW_CLASSIC : VIEW_NO_ROMS);
    if (palette().buttonText().color().valueF() > 0.5) {
      m_ui->rhdcViewToggleButton->setIcon(Icon::rhdcLight());
    } else {
      m_ui->rhdcViewToggleButton->setIcon(Icon::rhdcDark());
    }
    if (!RhdcCredentials::exists() && !DataProvider::hasRhdcData()) {
      m_ui->rhdcViewToggleButton->setVisible(false);
    }
  }
  romSelectionChanged();
}

void MainWindow::openSaveFileDirectory() const {
  FileExplorer::showFolder(BaseDir::data() / _NFS("retro-data") /
                           _NFS("saves"));
}

void MainWindow::openSavestateDirectory() const {
  FileExplorer::showFolder(BaseDir::data() / _NFS("retro-data") /
                           _NFS("states"));
}

void MainWindow::openSdCardDirectory() const {
  FileExplorer::showFolder(BaseDir::data() / _NFS("sdcard"));
}

void MainWindow::openDataDirectory() const {
  FileExplorer::showFolder(BaseDir::data());
}

void MainWindow::openConfigDirectory() const {
  FileExplorer::showFolder(BaseDir::config());
}

void MainWindow::openCacheDirectory() const {
  FileExplorer::showFolder(BaseDir::cache());
}

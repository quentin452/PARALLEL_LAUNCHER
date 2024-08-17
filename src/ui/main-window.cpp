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

#include "src/ui/icons.hpp"
#include "src/ui/keyboard-config-dialog.hpp"
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
  connect(m_ui->romList, SIGNAL(editSave(fs::path)), this,
          SLOT(editSave(fs::path)));
  connect(m_ui->rhdcPage, SIGNAL(editSave(fs::path)), this,
          SLOT(editSave(fs::path)));
  connect(m_ui->romList, SIGNAL(launchRom()), this, SLOT(playSingleplayer()));
  refreshRomList();

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
          if (refetch)
            this->refetchAll();
        });
      });
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

void MainWindow::refetchAll() {
  m_ui->romList->refetchAll();
  if (m_ui->romView->currentIndex() == VIEW_RHDC) {
    m_ui->rhdcPage->reload();
  } else {
    setView(m_ui->romList->hasRoms() ? VIEW_CLASSIC : VIEW_NO_ROMS);
  }
}
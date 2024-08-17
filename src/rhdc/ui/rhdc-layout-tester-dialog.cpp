#include "src/rhdc/ui/rhdc-layout-tester-dialog.hpp"
#include "ui_rhdc-layout-tester-dialog.h"

#include "src/core/retroarch.hpp"
#include "src/core/uuid.hpp"
#include "src/db/data-provider.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/play.hpp"
#include "src/ui/util.hpp"
#include <QByteArray>
#include <QFile>
#include <exception>
#include <sstream>

static StarLayout makePlaceholderLayout() {
  return StarLayout{/* numSlots */ 0,
                    /* slotsStart */ 0,
                    /* slotSize */ 0,
                    /* activeBit */ 0,
                    /* checksumOffset */ 0,
                    /* saveFormat */ StarLayout::SaveFormat::EEPROM,
                    /* groups */ {},
                    /* collectedStarIcon */ {},
                    /* missingStarIcon */ {}};
}

RhdcLayoutTesterDialog::RhdcLayoutTesterDialog(const fs::path &romPath)
    : QDialog(nullptr), m_ui(new Ui::RhdcLayoutTesterDialog),
      m_romPath(romPath), m_saveFilePath(RetroArch::getSaveFilePath(romPath)),
      m_originalSaveFile(BaseDir::temp() /
                         fs::to_path(Uuid::Random().toString() + ".srm")),
      m_idleCountdown(0) {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());
  UiUtil::fixDialogButtonsOnWindows(this);

  m_ui->closeButton->setIcon(Icon::close());
  m_ui->revertButton->setIcon(Icon::undo());
  m_ui->testButton->setIcon(Icon::play());

  if (!fs::existsSafe(m_saveFilePath)) {
    QFile saveFile(m_saveFilePath.u8string().c_str());
    saveFile.open(QFile::WriteOnly | QFile::Truncate);

    QByteArray zeroBuffer;
    zeroBuffer.fill('\0', 296960);

    saveFile.write(zeroBuffer);
    saveFile.flush();
  }

  fs::error_code err;
  fs::copy_file(m_saveFilePath, m_originalSaveFile,
                fs::copy_options::overwrite_existing, err);

  m_starDisplay = new EditableStarDisplayWidget(nullptr, m_saveFilePath,
                                                makePlaceholderLayout());
  m_starDisplay->makeScrollable();
  QLayoutItem *placeholder = m_ui->saveFileLayout->replaceWidget(
      m_ui->starDisplayPlaceholder, m_starDisplay, Qt::FindDirectChildrenOnly);
  m_ui->starDisplayPlaceholder->deleteLater();
  delete placeholder;

  m_ui->layoutTextArea->document()->setModified(false);
  m_ui->validationStatusLabel->clear();

  connect(&m_layoutChecker, &QTimer::timeout, this,
          &RhdcLayoutTesterDialog::checkLayout);
  m_layoutChecker.setInterval(500);
  m_layoutChecker.setSingleShot(false);
  m_layoutChecker.start();
}

RhdcLayoutTesterDialog::~RhdcLayoutTesterDialog() {
  m_starDisplay->deleteLater();
  delete m_ui;
}

void RhdcLayoutTesterDialog::closeEvent(QCloseEvent *event) {
  fs::error_code err;
  if (fs::existsSafe(m_originalSaveFile)) {
    fs::remove(m_saveFilePath, err);
  }
  fs::rename(m_originalSaveFile, m_saveFilePath, err);

  QDialog::closeEvent(event);

  m_layoutChecker.stop();
  deleteLater();
}

void RhdcLayoutTesterDialog::revertSave() {
  fs::error_code err;
  fs::copy_file(m_originalSaveFile, m_saveFilePath,
                fs::copy_options::overwrite_existing, err);
  m_starDisplay->load();
}

void RhdcLayoutTesterDialog::playtest() {
  m_starDisplay->save();

  RomInfo romInfo;
  RomFile romFile;
  if (!DataProvider::tryFetchRomByPath(m_romPath, true, &romInfo, &romFile)) {
    return;
  }

  hide();
  Game::play(
      romFile, romInfo, false, false,
      [=]() {
        this->m_starDisplay->load();
        this->show();
      },
      &m_starDisplay->starLayout());
}

void RhdcLayoutTesterDialog::layoutModificationStatusChanged(bool modified) {
  if (!modified)
    return;

  if (m_starDisplay->starLayout().numSlots != 0) {
    m_starDisplay->save();
  }

  updateStarDisplayLayout(makePlaceholderLayout());
  m_ui->testButton->setEnabled(false);
  m_ui->validationStatusLabel->setStyleSheet("font-style: italic;");
  m_ui->validationStatusLabel->setText("Waiting for you to finish typing...");
}

void RhdcLayoutTesterDialog::checkLayout() {
  QTextDocument *doc = m_ui->layoutTextArea->document();
  if (doc == nullptr)
    return;

  if (doc->isModified()) {
    m_idleCountdown = 2;
    doc->setModified(false);
  } else if (m_idleCountdown > 0 && --m_idleCountdown == 0) {
    validateLayout();
  }
}

void RhdcLayoutTesterDialog::validateLayout() {
  const string layoutString =
      m_ui->layoutTextArea->document()->toPlainText().toStdString();
  if (layoutString.empty()) {
    updateStarDisplayLayout(makePlaceholderLayout());
    m_ui->testButton->setEnabled(false);
    m_ui->validationStatusLabel->clear();
    return;
  }

  std::istringstream stream(layoutString);
  try {
    StarLayout layout = JsonSerializer::parse<StarLayout>(Json::parse(stream));
    updateStarDisplayLayout(std::move(layout));
    m_ui->testButton->setEnabled(true);
    m_ui->validationStatusLabel->setStyleSheet("");
    m_ui->validationStatusLabel->setText("✔️ Layout is valid.");
  } catch (const std::exception &ex) {
    updateStarDisplayLayout(makePlaceholderLayout());
    m_ui->testButton->setEnabled(false);
    m_ui->validationStatusLabel->setStyleSheet(
        "font-weight: bold; color: #C40000;");
    m_ui->validationStatusLabel->setText(
        ("Validation error: "s + ex.what()).c_str());
  }
}

void RhdcLayoutTesterDialog::updateStarDisplayLayout(StarLayout &&layout) {
  EditableStarDisplayWidget *newStarDisplay =
      new EditableStarDisplayWidget(nullptr, m_saveFilePath, std::move(layout));
  QLayoutItem *oldStarDisplay = m_ui->saveFileLayout->replaceWidget(
      m_starDisplay, newStarDisplay, Qt::FindDirectChildrenOnly);
  m_starDisplay->deleteLater();
  delete oldStarDisplay;
  m_starDisplay = newStarDisplay;
  m_starDisplay->makeScrollable();
}

void RhdcLayoutTesterDialog::reject() {
  QDialog::reject();
  close();
}

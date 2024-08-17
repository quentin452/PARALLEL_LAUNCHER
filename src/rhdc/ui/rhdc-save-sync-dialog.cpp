#include "src/rhdc/ui/rhdc-save-sync-dialog.hpp"
#include "ui_rhdc-save-sync-dialog.h"

#include "src/core/numeric-string.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include <QDateTime>

class SSDT_FileItem : public QTableWidgetItem {

private:
  const fs::path m_path;

public:
  SSDT_FileItem(const string &versionName, const fs::path &path)
      : QTableWidgetItem(QString(versionName.c_str()), 1001), m_path(path) {}

  virtual ~SSDT_FileItem() {}

  const fs::path &value() const { return m_path; }

  virtual bool operator<(const QTableWidgetItem &other) const override {
    return m_path < dynamic_cast<const SSDT_FileItem &>(other).m_path;
  }
};

class SSDT_NumericItem : public QTableWidgetItem {

private:
  const int64 m_value;

public:
  SSDT_NumericItem(const QString &text, int64 value)
      : QTableWidgetItem(text, 1002), m_value(value) {}

  virtual ~SSDT_NumericItem() {}

  virtual bool operator<(const QTableWidgetItem &other) const override {
    return m_value < dynamic_cast<const SSDT_NumericItem &>(other).m_value;
  }
};

RhdcSaveSyncDialog::RhdcSaveSyncDialog(
    const std::vector<SaveFileInfo> &saveFiles)
    : QDialog(nullptr), m_ui(new Ui::RhdcSaveSyncDialog) {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());
  UiUtil::fixDialogButtonsOnWindows(this);

  m_ui->table->setRowCount((int)saveFiles.size());
  m_ui->table->setSortingEnabled(false);

  int row = 0;
  int suggestedRow = 0;

  const SaveFileInfo *suggestedSaveFile = nullptr;
  for (const SaveFileInfo &saveFile : saveFiles) {
    m_ui->table->setItem(
        row, 0, new SSDT_FileItem(saveFile.versionName, saveFile.path));
    m_ui->table->setItem(
        row, 1,
        new SSDT_NumericItem(
            QDateTime::fromSecsSinceEpoch(saveFile.lastModified)
                .toString("yyyy-MM-dd h:mm AP"),
            saveFile.lastModified));
    m_ui->table->setItem(
        row, 2,
        new SSDT_NumericItem(Number::toString(saveFile.starCount).c_str(),
                             (int64)saveFile.starCount));

    if (suggestedSaveFile == nullptr) {
      suggestedSaveFile = &saveFile;
      suggestedRow = row;
    } else if (saveFile.starCount > suggestedSaveFile->starCount ||
               (saveFile.starCount == suggestedSaveFile->starCount &&
                saveFile.lastModified > suggestedSaveFile->lastModified)) {
      suggestedSaveFile = &saveFile;
      suggestedRow = row;
    }

    row++;
  }

  m_ui->table->sortByColumn(0, Qt::AscendingOrder);
  m_ui->table->setSortingEnabled(true);

  m_ui->table->setCurrentCell(suggestedRow, 0);

  m_ui->table->horizontalHeader()->setStretchLastSection(true);
  m_ui->table->horizontalHeader()->resizeSections(
      QHeaderView::ResizeToContents);

  const int sizeHint = m_ui->table->horizontalHeader()->length() + 68;
  if (sizeHint > width()) {
    resize(sizeHint > 1200 ? 1200 : sizeHint, height());
  }
}

fs::path RhdcSaveSyncDialog::selectedSaveFile() const {
  const int selectedRow = m_ui->table->currentRow();
  if (selectedRow < 0)
    return fs::path();

  const SSDT_FileItem *item =
      dynamic_cast<const SSDT_FileItem *>(m_ui->table->item(selectedRow, 0));
  if (item == nullptr)
    return fs::path();

  return item->value();
}

fs::path RhdcSaveSyncDialog::run(const std::vector<SaveFileInfo> &saveFiles) {
  if (saveFiles.empty())
    return fs::path();

  RhdcSaveSyncDialog dialog(saveFiles);
  if (dialog.exec() == QDialog::Accepted) {
    return dialog.selectedSaveFile();
  } else {
    return fs::path();
  }
}

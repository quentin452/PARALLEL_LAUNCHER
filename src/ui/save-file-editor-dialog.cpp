#include "src/ui/save-file-editor-dialog.hpp"
#include "ui_save-file-editor-dialog.h"

#include "src/core/numeric-string.hpp"
#include "src/polyfill/file.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include <QCoreApplication>
#include <cstring>

static inline QString getSlotName(int slotIndex,
                                  const SM64::SaveFile &saveFile) {
  const SM64::SaveSlot &slot = saveFile.slot(slotIndex);
  const char slotChar[2] = {(char)('A' + (char)slotIndex), 0};
  if (!Flags::has(slot.flags, SM64::SaveFileFlag::Exists)) {
    return SaveFileEditorDialog::tr("Slot %1 (Empty)").arg(slotChar);
  }

  return SaveFileEditorDialog::tr("Slot %1 (%2 Stars)")
      .arg(slotChar)
      .arg(slot.countStars());
}

static inline void commitChanges(const fs::path &filePath,
                                 const SM64::SaveFile &saveData) {
  OutputFile file(filePath, true);
  saveData.write(file);
}

SaveFileEditorDialog::SaveFileEditorDialog(const fs::path &saveFilePath)
    : QDialog(nullptr), m_ui(new Ui::SaveFileEditorDialog),
      m_filePath(saveFilePath) {
  m_ui->setupUi(this);
  UiUtil::scaleWindow(this);
  UiUtil::fixDialogButtonsOnWindows(this);

  InputFile file(saveFilePath, true);
  m_saveFile = SM64::SaveFile::read(file);

  for (int i = 0; i < 4; i++) {
    m_ui->saveSlot->setItemText(i, getSlotName(i, m_saveFile));
  }

  m_ui->deleteButton->setIcon(Icon::delet());
  m_ui->editButton->setIcon(Icon::edit());
  setWindowIcon(Icon::appIcon());

  m_ui->saveSlot->setCurrentIndex(0);
  slotSelected(0);
}

SaveFileEditorDialog::~SaveFileEditorDialog() { delete m_ui; }

int SaveFileEditorDialog::getSaveSlot() const {
  return m_ui->saveSlot->currentIndex();
}

void SaveFileEditorDialog::slotSelected(int slotIndex) {
  const SM64::SaveSlot &slot = m_saveFile.slot(slotIndex);
  if (Flags::has(slot.flags, SM64::SaveFileFlag::Exists)) {
    m_ui->deleteButton->setVisible(true);
    m_ui->editButton->setText(tr("Edit Save Slot"));
    m_ui->editButton->setIcon(Icon::edit());
  } else {
    m_ui->deleteButton->setVisible(false);
    m_ui->editButton->setText(tr("Create Save Slot"));
    m_ui->editButton->setIcon(Icon::add());
  }
}

void SaveFileEditorDialog::deleteSlot() {
  const int slotIndex = m_ui->saveSlot->currentIndex();
  SM64::SaveSlot &slot = m_saveFile.slot(slotIndex);
  std::memset(&slot, 0, sizeof(SM64::SaveSlot));
  commitChanges(m_filePath, m_saveFile);
  m_ui->saveSlot->setItemText(slotIndex, getSlotName(slotIndex, m_saveFile));
  slotSelected(slotIndex);
}

void SaveFileEditorDialog::accept() {
  SM64::SaveSlot &slot = m_saveFile.slot(m_ui->saveSlot->currentIndex());
  if (!Flags::has(slot.flags, SM64::SaveFileFlag::Exists)) {
    std::memset(&slot, 0, sizeof(SM64::SaveSlot));
    slot.flags = SM64::SaveFileFlag::Exists;
    commitChanges(m_filePath, m_saveFile);
  }

  QDialog::accept();
}

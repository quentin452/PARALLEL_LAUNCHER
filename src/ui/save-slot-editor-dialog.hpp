#ifndef SRC_UI_SAVE_SLOT_EDITOR_DIALOG_HPP_
#define SRC_UI_SAVE_SLOT_EDITOR_DIALOG_HPP_

#include "src/core/filesystem.hpp"
#include <QDialog>
#include <QLabel>
#include <QSpinBox>

namespace Ui {
class SaveSlotEditorDialog;
}

class SaveSlotEditorDialog : public QDialog {
  Q_OBJECT

private:
  Ui::SaveSlotEditorDialog *m_ui;
  const fs::path m_filePath;
  const int m_slot;
  QSpinBox *m_coinInputs[15];

public:
  explicit SaveSlotEditorDialog(const fs::path &saveFilePath, int slot);
  virtual ~SaveSlotEditorDialog();

private slots:
  void setShowUnused(bool showUnused);

public slots:
  virtual void accept() override;
};

#endif /* SRC_UI_SAVE_SLOT_EDITOR_DIALOG_HPP_ */

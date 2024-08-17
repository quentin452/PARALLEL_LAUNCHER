#ifndef SRC_UI_SAVE_FILE_EDITOR_DIALOG_HPP_
#define SRC_UI_SAVE_FILE_EDITOR_DIALOG_HPP_

#include "src/core/filesystem.hpp"
#include "src/core/sm64.hpp"
#include <QDialog>

namespace Ui {
class SaveFileEditorDialog;
}

class SaveFileEditorDialog : public QDialog {
  Q_OBJECT

private:
  Ui::SaveFileEditorDialog *m_ui;
  const fs::path m_filePath;
  SM64::SaveFile m_saveFile;

public:
  explicit SaveFileEditorDialog(const fs::path &saveFilePath);
  virtual ~SaveFileEditorDialog();

  int getSaveSlot() const;

private slots:
  void slotSelected(int slotIndex);
  void deleteSlot();

public slots:
  virtual void accept() override;
};

#endif /* SRC_UI_SAVE_FILE_EDITOR_DIALOG_HPP_ */

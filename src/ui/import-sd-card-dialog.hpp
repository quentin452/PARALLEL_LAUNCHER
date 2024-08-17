#ifndef SRC_UI_IMPORT_SD_CARD_DIALOG_HPP_
#define SRC_UI_IMPORT_SD_CARD_DIALOG_HPP_

#include "src/core/filesystem.hpp"
#include "src/types.hpp"
#include <QDialog>
#include <unordered_set>

namespace Ui {
class ImportSdCardDialog;
}

class ImportSdCardDialog : public QDialog {
  Q_OBJECT

private:
  const fs::path m_filePath;
  Ui::ImportSdCardDialog *m_ui;
  HashSet<string> m_usedNames;

public:
  ImportSdCardDialog(QWidget *parent, const fs::path &filePath);
  ~ImportSdCardDialog();

private slots:
  void nameChanged(QString name);

public slots:
  virtual void accept() override;
};

#endif /* SRC_UI_IMPORT_SD_CARD_DIALOG_HPP_ */

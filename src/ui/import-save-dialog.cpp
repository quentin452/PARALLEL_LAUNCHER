#include "src/ui/import-save-dialog.hpp"

#include "src/core/save-converter.hpp"
#include "src/polyfill/file-dialog.hpp"
#include "src/types.hpp"
#include "src/ui/toast.hpp"
#include <QCoreApplication>

void ImportSaveDialog::exec(const fs::path &saveFileDestination) {
  static const string dialogTitle =
      QCoreApplication::translate("ImportSaveDialog", "Select Save File")
          .toStdString();
  static const string fileType =
      QCoreApplication::translate("ImportSaveDialog", "Project64 Save Files")
          .toStdString();

  const fs::path sourceFilePath = FileDialog::getFile(
      dialogTitle.c_str(), {{fileType.c_str(), "*.eep;*.mpk;*.sra;*.fla"}});

  if (sourceFilePath.empty())
    return;

  if (SaveConverter::importFromProject64(saveFileDestination, sourceFilePath)) {
    static const QString successMsg =
        QCoreApplication::translate("ImportSaveDialog", "Save data imported");
    ToastMessageManager::display(ToastType::Info, successMsg);
  } else {
    static const QString failureMsg = QCoreApplication::translate(
        "ImportSaveDialog", "Failed to import save data");
    ToastMessageManager::display(ToastType::Error, failureMsg);
  }
}

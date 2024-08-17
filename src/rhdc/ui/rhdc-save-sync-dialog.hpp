#ifndef SRC_RHDC_UI_RHDC_SAVE_SYNC_DIALOG_HPP_
#define SRC_RHDC_UI_RHDC_SAVE_SYNC_DIALOG_HPP_

#include "src/core/filesystem.hpp"
#include "src/types.hpp"
#include <QDialog>
#include <vector>

struct SaveFileInfo {
  fs::path path;
  string versionName;
  int64 lastModified;
  ushort starCount;
};

namespace Ui {
class RhdcSaveSyncDialog;
}

class RhdcSaveSyncDialog : protected QDialog {
  Q_OBJECT

private:
  Ui::RhdcSaveSyncDialog *m_ui;

  RhdcSaveSyncDialog(const std::vector<SaveFileInfo> &saveFiles);
  ~RhdcSaveSyncDialog() {}

  fs::path selectedSaveFile() const;

public:
  static fs::path run(const std::vector<SaveFileInfo> &saveFiles);
};

#endif /* SRC_RHDC_UI_RHDC_SAVE_SYNC_DIALOG_HPP_ */

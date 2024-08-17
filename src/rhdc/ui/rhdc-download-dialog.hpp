#ifndef SRC_RHDC_UI_RHDC_DOWNLOAD_DIALOG_HPP_
#define SRC_RHDC_UI_RHDC_DOWNLOAD_DIALOG_HPP_

#include "src/core/filesystem.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/types.hpp"
#include <QDialog>
#include <memory>

namespace Ui {
class RhdcDownloadDialog;
}

class RhdcDownloadDialog : private QDialog {
  Q_OBJECT

private:
  Ui::RhdcDownloadDialog *m_ui;

  RhdcDownloadDialog();
  ~RhdcDownloadDialog();

  void getRecommendedPlugin(const string &hackId, const string &downloadUrl,
                            const string &versionName,
                            std::shared_ptr<bool> dialogExists);

  void downloadLayout(const string &hackId, const string &hackDownloadUrl,
                      const PluginAndLayoutInfo &hackInfo,
                      std::shared_ptr<bool> dialogExists);

  void downloadHack(const string &hackId, const string &downloadUrl,
                    const PluginAndLayoutInfo &hackInfo,
                    std::shared_ptr<bool> dialogExists);

  static void patchRom(RhdcDownloadDialog *dialog, string hackId,
                       fs::path patchPath, fs::path baseRomPath,
                       PluginAndLayoutInfo hackInfo,
                       std::shared_ptr<bool> dialogExists);

public:
  static fs::path run(const string &hackId, const string &downloadUrl,
                      const string &versionName);

  static fs::path run(const string &hackId, const string &downloadUrl);

  static fs::path runForRhdcUrl(const string &rhdcUrl);
};

#endif /* SRC_RHDC_UI_RHDC_DOWNLOAD_DIALOG_HPP_ */

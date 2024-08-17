#ifndef SRC_RHDC_UI_RHDC_LOGIN_DIALOG_HPP_
#define SRC_RHDC_UI_RHDC_LOGIN_DIALOG_HPP_

#include <QDialog>
#include <memory>

namespace Ui {
class RhdcLoginDialog;
}

class RhdcLoginDialog : public QDialog {
  Q_OBJECT

private:
  Ui::RhdcLoginDialog *m_ui;
  std::shared_ptr<bool> m_exists;

public:
  RhdcLoginDialog(QWidget *parent = nullptr);
  virtual ~RhdcLoginDialog();

protected:
  virtual void showEvent(QShowEvent *event) override;

public slots:
  virtual void accept() override;

private slots:
  void browseForRom();
  void browseForDir();
  void login();

  void validateMfaCode();
  void sendMfaCode();
};

#endif /* SRC_RHDC_UI_RHDC_LOGIN_DIALOG_HPP_ */

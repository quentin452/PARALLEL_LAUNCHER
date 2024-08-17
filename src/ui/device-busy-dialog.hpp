#ifndef SRC_UI_DEVICE_BUSY_DIALOG_HPP_
#define SRC_UI_DEVICE_BUSY_DIALOG_HPP_

#include <QDialog>

namespace Ui {
class DeviceBusyDialog;
}

class DeviceBusyDialog : public QDialog {
  Q_OBJECT

private:
  Ui::DeviceBusyDialog *m_ui;

public:
  DeviceBusyDialog(QWidget *parent = nullptr);
  ~DeviceBusyDialog();
};

#endif /* SRC_UI_DEVICE_BUSY_DIALOG_HPP_ */

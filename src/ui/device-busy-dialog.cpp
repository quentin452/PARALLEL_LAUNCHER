#include "src/ui/device-busy-dialog.hpp"
#include "ui_device-busy-dialog.h"

#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

DeviceBusyDialog::DeviceBusyDialog(QWidget *parent)
    : QDialog(parent), m_ui(new Ui::DeviceBusyDialog) {
  m_ui->setupUi(this);

  setWindowIcon(Icon::appIcon());
  UiUtil::scaleWindow(this);

  m_ui->unmountButton->setIcon(Icon::eject());
  m_ui->cancelButton->setIcon(Icon::cancel());
}

DeviceBusyDialog::~DeviceBusyDialog() { delete m_ui; }

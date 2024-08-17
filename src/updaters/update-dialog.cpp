#include "src/updaters/update-dialog.hpp"

#ifndef __linux__
#include "ui_update-dialog.h"

#include "src/core/version.hpp"
#include "src/ui/icons.hpp"

UpdateDialog::UpdateDialog(const Version &latestVersion,
                           const QString &changelog)
    : QDialog(nullptr), m_ui(new Ui::UpdateDialog),
      m_latestVersion(latestVersion) {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());

  m_ui->changelog->setHtml(changelog);
  m_ui->versionLabel->setText(("v"s + CurrentVersion::Application.toString() +
                               " → v" + m_latestVersion.toString())
                                  .c_str());
}

UpdateDialog::~UpdateDialog() { delete m_ui; }

bool UpdateDialog::stopReminders() const {
  return m_ui->noReminderCheckbox->isChecked();
}

#endif

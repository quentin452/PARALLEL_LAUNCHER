#include "src/ui/crash-report-dialog.hpp"
#include "ui_crash-report-dialog.h"

#include "src/db/data-provider.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include <QDateTime>

CrashReportDialog::CrashReportDialog(QWidget *parent)
    : QDialog(parent), m_ui(new Ui::CrashReportDialog), m_reports() {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());
  UiUtil::fixDialogButtonsOnWindows(this);

  versionFilterToggled(true);
  reportSelected();
}

CrashReportDialog::~CrashReportDialog() { delete m_ui; }

void CrashReportDialog::versionFilterToggled(bool filter) {
  m_reports =
      filter ? DataProvider::getCrashLogsForVersion(CurrentVersion::Application)
             : DataProvider::getAllCrashLogs();

  m_ui->reportList->clear();
  for (auto i = m_reports.crbegin(); i != m_reports.crend(); i++) {
    m_ui->reportList->addItem(
        QDateTime::fromSecsSinceEpoch(i->first).toString());
    m_ui->reportList->item(m_ui->reportList->count() - 1)
        ->setData(Qt::UserRole, QVariant::fromValue(i->first));
  }

  m_ui->reportList->setCurrentRow(0);
  reportSelected();
}

void CrashReportDialog::reportSelected() {
  m_ui->crashReportView->clear();

  const QListWidgetItem *item = m_ui->reportList->currentItem();
  if (!item)
    return;

  const auto r = m_reports.find(item->data(Qt::UserRole).toLongLong());
  if (r == m_reports.end())
    return;

  m_ui->crashReportView->setPlainText(QString::fromStdString(r->second));
}

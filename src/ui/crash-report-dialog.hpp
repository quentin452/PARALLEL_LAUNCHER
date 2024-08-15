#ifndef SRC_UI_CRASH_REPORT_DIALOG_HPP_
#define SRC_UI_CRASH_REPORT_DIALOG_HPP_

#include <QDialog>
#include <map>
#include "src/types.hpp"

namespace Ui {
	class CrashReportDialog;
}

class CrashReportDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::CrashReportDialog *m_ui;
	std::map<int64,string> m_reports;

	public:
	CrashReportDialog( QWidget *parent = nullptr );
	~CrashReportDialog();

	private slots:
	void versionFilterToggled( bool filter );
	void reportSelected();

};

#endif /* SRC_UI_CRASH_REPORT_DIALOG_HPP_ */

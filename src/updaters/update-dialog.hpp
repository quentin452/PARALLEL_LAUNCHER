#ifndef SRC_UPDATERS_UPDATE_DIALOG_HPP_
#define SRC_UPDATERS_UPDATE_DIALOG_HPP_

#ifndef __linux__

#include <QDialog>
#include "src/core/version.hpp"

namespace Ui {
	class UpdateDialog;
}

class UpdateDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::UpdateDialog *m_ui;
	const Version &m_latestVersion;

	public:
	UpdateDialog( const Version &latestVersion, const QString &changelog );
	~UpdateDialog();

	bool stopReminders() const;

};

#endif

#endif /* SRC_UPDATERS_UPDATE_DIALOG_HPP_ */

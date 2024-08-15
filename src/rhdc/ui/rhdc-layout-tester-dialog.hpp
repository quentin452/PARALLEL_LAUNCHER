#ifndef SRC_RHDC_UI_RHDC_LAYOUT_TESTER_DIALOG_HPP_
#define SRC_RHDC_UI_RHDC_LAYOUT_TESTER_DIALOG_HPP_

#include <QDialog>
#include <QTimer>
#include "src/rhdc/ui/star-display-widget.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/core/filesystem.hpp"

namespace Ui {
	class RhdcLayoutTesterDialog;
}

class RhdcLayoutTesterDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::RhdcLayoutTesterDialog *m_ui;
	EditableStarDisplayWidget *m_starDisplay;
	QTimer m_layoutChecker;
	const fs::path m_romPath;
	const fs::path m_saveFilePath;
	const fs::path m_originalSaveFile;
	int m_idleCountdown;

	void validateLayout();
	void updateStarDisplayLayout( StarLayout &&layout );

	protected:
	virtual void closeEvent( QCloseEvent *event ) override;

	public:
	RhdcLayoutTesterDialog( const fs::path &romPath );
	virtual ~RhdcLayoutTesterDialog();

	public slots:
	virtual void reject() override;

	private slots:
	void revertSave();
	void playtest();
	void layoutModificationStatusChanged( bool modified );
	void checkLayout();

};



#endif /* SRC_RHDC_UI_RHDC_LAYOUT_TESTER_DIALOG_HPP_ */

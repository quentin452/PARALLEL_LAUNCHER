#ifndef SRC_UI_SETTINGS_DIALOG_HPP_
#define SRC_UI_SETTINGS_DIALOG_HPP_

#include <QDialog>

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::SettingsDialog *m_ui;

	protected:
	virtual void showEvent( QShowEvent *event ) override;
	virtual void closeEvent( QCloseEvent *event ) override;

	private slots:
	void save();
	void windowScaleChanged( int index );
	void browseForBpsPath();
	void browseForRhdcDir();
	void resetConfig();
	void updateCores();
	void applyAll_accurateDepthCompare();
	void applyAll_emulateFramebuffer();
	void applyAll_upscaleTexRects();
	void applyAll_removeBorders();
	void fbEmulationToggled( bool enabled );
	void historySizeChanged( int value );
	void rtcSyncToggled( bool syncToClock );

	public:
	SettingsDialog( QWidget *parent = nullptr );
	~SettingsDialog();

};

#endif /* SRC_UI_SETTINGS_DIALOG_HPP_ */

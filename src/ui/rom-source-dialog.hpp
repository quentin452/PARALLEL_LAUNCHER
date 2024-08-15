#ifndef SRC_UI_ROM_SOURCE_DIALOG_HPP_
#define SRC_UI_ROM_SOURCE_DIALOG_HPP_

#include <unordered_map>
#include <QDialog>
#include <QCheckBox>
#include "src/core/rom.hpp"

namespace Ui {
	class RomSourceDialog;
}

class RomSourceDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::RomSourceDialog *m_ui;
	std::vector<RomSource> m_sources;
	HashMap<string,QCheckBox*> m_groupMap;
	RomSource *m_currentSource;
	bool m_ignoreEvents;

	void saveCurrentSource();
	void refreshGroups();
	void refreshApplyPatchButton();
	void addRomToList( const QString &path );

	public:
	explicit RomSourceDialog( QWidget *parent = nullptr );
	virtual ~RomSourceDialog();

	protected:
	virtual void showEvent( QShowEvent *event ) override;
	virtual void closeEvent( QCloseEvent *event ) override;

	public slots:
	virtual void accept() override;

	private slots:
	void addSource();
	void deleteSource();
	void addRom();
	void forgetRom();
	void sourceSelected();
	void romSelected();
	void browse();
	void manageGroups();
	void save();
	void browseForPatch();
	void browseForBaseRom();
	void applyPatch();
	void baseRomModeChanged();

};



#endif /* SRC_UI_ROM_SOURCE_DIALOG_HPP_ */

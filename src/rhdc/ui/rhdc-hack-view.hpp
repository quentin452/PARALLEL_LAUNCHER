#ifndef SRC_RHDC_UI_RHDC_HACK_VIEW_HPP_
#define SRC_RHDC_UI_RHDC_HACK_VIEW_HPP_

#include <QWidget>
#include "src/rhdc/core/hack.hpp"
#include "src/rhdc/ui/rhdc-ui-state.hpp"
#include "src/core/filesystem.hpp"
#include "src/ui/rom-settings-widget.hpp"

namespace Ui {
	class RhdcHackView;
}

class RhdcHackView : public QWidget {
	Q_OBJECT

	private:
	Ui::RhdcHackView *m_ui;
	bool m_sortAsc;
	bool m_blockPlaylistEvents = false;
	bool m_blockVersionEvents = false;

	void updatePlaylists();
	void reloadInternal( bool reshuffle );

	protected:
	virtual void showEvent( QShowEvent *event );

	public:
	RhdcHackView( QWidget *parent = nullptr );
	~RhdcHackView();

	RomSettingsWidgetSlot *settingsSlot() const;

	RhdcUiState uiState() const;
	string getSelectedVersion() const;
	string getCurrentPlaylist() const;

	bool tryGetHackIdAndDownloadUrl( string &hackId, string &downloadUrl ) const;
	string getSelectedHackId() const;

	private slots:
	void changeSortOrder();
	void versionSelected();
	void sortingChanged();
	void playlistChanged();
	void actionButtonClicked();

	public slots:
	void hackSelected( const RhdcHackExt *hack );
	void reload();

	signals:
	void romSelected();
	void editSave( fs::path saveFilePath );

};



#endif /* SRC_RHDC_UI_RHDC_HACK_VIEW_HPP_ */

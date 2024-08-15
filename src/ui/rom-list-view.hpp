#ifndef SRC_UI_ROM_LIST_VIEW_HPP_
#define SRC_UI_ROM_LIST_VIEW_HPP_

#include <QTreeView>
#include <QAbstractItemDelegate>
#include <unordered_set>
#include "src/core/rom.hpp"
#include "src/ui/ui-state.hpp"

class RomListRenderer : public QAbstractItemDelegate {
	Q_OBJECT

	private:
	QAbstractItemDelegate *const m_inner;

	public:
	RomListRenderer( QAbstractItemDelegate *innerDelegate );
	inline virtual ~RomListRenderer() {}

	inline virtual QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const override {
		return m_inner->createEditor( parent, option, index );
	}
	inline virtual void destroyEditor( QWidget *editor, const QModelIndex &index ) const override {
		m_inner->destroyEditor( editor, index );
	}
	inline virtual bool editorEvent( QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index ) override {
		return m_inner->editorEvent( event, model, option, index );
	}
	inline virtual bool helpEvent( QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index ) override {
		return m_inner->helpEvent( event, view, option, index );
	}
	inline virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override {
		m_inner->setEditorData( editor, index );
	}
	inline virtual void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const override {
		m_inner->setModelData( editor, model, index );
	}
	inline virtual void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const override {
		m_inner->updateEditorGeometry( editor, option, index );
	}

	virtual void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
	virtual QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const override;

};

class RomListView : public QTreeView {
	Q_OBJECT

	private:
	HashSet<string> m_expanded;

	private slots:
	void onRightClick( const QPoint &point );
	void onDoubleClick( const QModelIndex &index );

	public slots:
	void search( QString search );

	protected:
	virtual void keyPressEvent( QKeyEvent *event );

	public:
	explicit RomListView( QWidget *parent = nullptr );
	virtual ~RomListView();

	RomReference tryGetSelectedRom() const;
	bool hasRoms() const;

	void refetchAll();
	void refetchRhdc();
	void updateRomInfo( const RomInfo &romInfo, bool nameChanged, bool playTimeChanged );

	bool selectRom( const string &group, const fs::path &rom );

	TreeUiState saveTreeState();
	void restoreTreeState( const TreeUiState &state );

	RomInfo *tryGetRomInfo( const string &sha1 ) const;

	signals:
	void editSave( fs::path saveFilePath );
	void launchRom();
	void downloadPatch();
	void selectionFilteredOut();

};


#endif /* SRC_UI_ROM_LIST_VIEW_HPP_ */

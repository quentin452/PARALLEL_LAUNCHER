#ifndef SRC_RHDC_UI_RHDC_HACK_LIST_VIEW_HPP_
#define SRC_RHDC_UI_RHDC_HACK_LIST_VIEW_HPP_

#include <QStyledItemDelegate>
#include <QAbstractItemModel>
#include <QListView>
#include <vector>
#include <unordered_map>
#include "src/rhdc/ui/rhdc-hack-widget.hpp"
#include "src/rhdc/ui/rhdc-ui-state.hpp"
#include "src/types.hpp"

class RhdcHackRenderer : public QStyledItemDelegate {
	Q_OBJECT

	public:
	RhdcHackRenderer( QObject *parent = nullptr );
	~RhdcHackRenderer();

	virtual QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
	virtual void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
	virtual QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const override;

};

class RhdcHackListView : public QListView {
	Q_OBJECT

	protected:
	virtual void focusInEvent( QFocusEvent *event );
	virtual void resizeEvent( QResizeEvent *event );

	private slots:
	void modelIndexChanged( const QModelIndex&, const QModelIndex& );
	void searchChanged( QString search );

	public:
	RhdcHackListView( QWidget *parent = nullptr );
	~RhdcHackListView();

	void setPlaylist( const string &playlist );
	inline void clearPlaylist() { setPlaylist( "" ); }

	void reloadAllHacks( bool reshuffle );
	void reloadHack( const string &hackId );
	void setSortOrder( RhdcSorting sortBy, bool ascending );

	const RhdcHackExt *currentHack() const;
	void selectHack( const string &hackId );

	signals:
	void selectedHackChanged( const RhdcHackExt* );
	void resized( int editorWidth );

};

class RhdcHackListModel : public QAbstractItemModel {
	Q_OBJECT

	private:
	string m_playlist;
	std::vector<RhdcHackWidget*> m_hacks;
	HashMap<string,int> m_indexMap;
	std::string m_search;
	RhdcSorting m_sortBy;
	bool m_sortAsc;
	uint64 m_shuffleSeed;

	void doSort();

	public:
	RhdcHackListModel( QObject *parent = nullptr );
	~RhdcHackListModel();

	virtual QVariant data( const QModelIndex &index, int role ) const override;
	virtual Qt::ItemFlags flags( const QModelIndex &index ) const override;
	virtual QModelIndex index( int row, int column, const QModelIndex &parent ) const override;
	virtual QModelIndex parent( const QModelIndex &index ) const override;
	virtual int rowCount( const QModelIndex &parent ) const override;
	virtual int columnCount( const QModelIndex &parent ) const override;

	void setSearch( const QString &search );
	void setPlaylist( const string &playlist );
	void reloadOne( const string &hackId );
	void reloadAll();
	void setSortOrder( RhdcSorting sortBy, bool ascending );
	QModelIndex findHack( const string &hackId ) const;

	inline void prepReshuffle();

};

#endif /* SRC_RHDC_UI_RHDC_HACK_LIST_VIEW_HPP_ */

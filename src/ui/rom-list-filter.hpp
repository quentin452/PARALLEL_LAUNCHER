#ifndef SRC_UI_ROM_LIST_FILTER_HPP_
#define SRC_UI_ROM_LIST_FILTER_HPP_

#include <QSortFilterProxyModel>
#include "src/core/rom.hpp"

class RomListFilter : protected QSortFilterProxyModel {
	Q_OBJECT

	private:
	QString m_filter;

	public:
	RomListFilter( QAbstractItemModel *model ) : QSortFilterProxyModel( model->parent() ), m_filter() {
		setSourceModel( model );
		setRecursiveFilteringEnabled( true );
	}

	virtual ~RomListFilter() {
		sourceModel()->deleteLater();
	}

	void fullUpdate();
	void rhdcUpdate();
	void partialUpdate( const RomInfo &changedRom, bool nameChanged, bool playTimeChanged );

	string tryGetGroup( const QModelIndex &index ) const;
	RomReference tryGetRom( const QModelIndex &index );
	ConstRomReference tryGetRom( const QModelIndex &index ) const;
	QModelIndex tryGetIndex( const string &group, const fs::path &rom ) const;
	QModelIndex tryGetIndex( const string &group ) const;
	RomInfo *tryGetRomInfo( const string &sha1 );
	bool hasRoms() const noexcept;

	inline void setFilter( const QString &filter ) {
		m_filter = filter;
		invalidate();
	}

	inline void clearFilter() {
		if( m_filter.isEmpty() ) return;
		m_filter.clear();
		invalidate();
	}

	protected:
	virtual bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const override;
	virtual QVariant data( const QModelIndex &index, int role ) const override;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;

};

#endif /* SRC_UI_ROM_LIST_FILTER_HPP_ */

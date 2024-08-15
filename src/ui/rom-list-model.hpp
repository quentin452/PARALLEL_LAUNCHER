#ifndef SRC_UI_ROM_LIST_MODEL_HPP_
#define SRC_UI_ROM_LIST_MODEL_HPP_

#include <QAbstractItemModel>
#include <QSize>
#include <vector>
#include <array>
#include <unordered_map>
#include "src/core/rom.hpp"
#include "src/rhdc/core/hack.hpp"

namespace __RomListModelPrivate {
	static constexpr int NUM_COLUMNS = 5;

	struct _RomListModelIndex {
		int group;
		int row;
		int col;
		int lookupIndex;
	};

	typedef std::array<_RomListModelIndex,NUM_COLUMNS> _RomListModelRow;

	struct _RomListModelGroup {
		string name;
		_RomListModelIndex self;
		std::vector<_RomListModelRow> children;
	};
}

class RomListModel : public QAbstractItemModel {
	Q_OBJECT

	private:
	std::vector<RomFile> m_romFiles;
	HashMap<string,RomInfo> m_romInfo;
	HashMap<string,string> m_rhdcMap;
	HashMap<string,BasicRhdcHack> m_rhdcInfo;
	std::vector<__RomListModelPrivate::_RomListModelGroup> m_groups;
	HashMap<string,std::vector<__RomListModelPrivate::_RomListModelRow*>> m_reverseLookup;
	int m_sortBy;
	Qt::SortOrder m_sortOrder;

	static constexpr int COL_NAME = 0;
	static constexpr int COL_INTERNAL_NAME = 1;
	static constexpr int COL_PATH = 2;
	static constexpr int COL_LAST_PLAYED = 3;
	static constexpr int COL_PLAY_TIME = 4;

	void sortInternal();
	BasicRhdcHack *tryGetRhdcHackWithSha1( const string &sha1 );
	const BasicRhdcHack *tryGetRhdcHackWithSha1( const string &sha1 ) const;

	public:
	RomListModel( QObject *parent ) :
		QAbstractItemModel( parent ),
		m_sortBy( COL_NAME ),
		m_sortOrder( Qt::SortOrder::AscendingOrder )
	{}

	virtual ~RomListModel() {}

	void fullUpdate();
	void rhdcUpdate();
	void partialUpdate( const RomInfo &changedRom, bool nameChanged, bool playTimeChanged );

	string tryGetGroup( const QModelIndex &index ) const;
	RomReference tryGetRom( const QModelIndex &index );
	ConstRomReference tryGetRom( const QModelIndex &index ) const;
	QModelIndex tryGetIndex( const string &group, const fs::path &rom ) const;
	QModelIndex tryGetIndex( const string &group ) const;

	RomInfo *tryGetRomInfo( const string &sha1 );

	inline bool hasRoms() const noexcept {
		return !m_romFiles.empty();
	}

	virtual int columnCount( [[maybe_unused]] const QModelIndex &parent ) const override { return __RomListModelPrivate::NUM_COLUMNS; }
	virtual QSize span( const QModelIndex &index ) const override {
		return QSize( hasChildren( index ) ? __RomListModelPrivate::NUM_COLUMNS : 0, 1 );
	}

	virtual QVariant data( const QModelIndex &index, int role ) const override;
	virtual Qt::ItemFlags flags( const QModelIndex &index ) const override;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
	virtual QModelIndex index( int row, int column, const QModelIndex &parent ) const override;
	virtual QModelIndex parent( const QModelIndex &index ) const override;
	virtual bool hasChildren( const QModelIndex &parent ) const override;
	virtual int rowCount( const QModelIndex &parent ) const override;
	virtual void sort( int column, Qt::SortOrder order ) override;

};



#endif /* SRC_UI_ROM_LIST_MODEL_HPP_ */

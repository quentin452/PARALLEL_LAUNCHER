#include "src/rhdc/ui/rhdc-hack-list-view.hpp"

#include <QScrollBar>
#include <algorithm>
#include "src/polyfill/random.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/qthread.hpp"
#include "src/db/data-provider.hpp"

static inline double getDifficulty( const RhdcHackExt &hack ) {
	const double difficulty = (hack.progress.difficulty > 0) ? (double)hack.progress.difficulty : hack.info.avgDifficulty;
	return (hack.info.category == "Kaizo") ? difficulty + 10.0 : difficulty;
}

static inline double getRating( const RhdcHackExt &hack ) {
	return hack.info.avgRating;
}

static bool compareName( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return QString::compare( a->hack().info.name.c_str() , b->hack().info.name.c_str(), Qt::CaseInsensitive ) < 0;
}

static bool comparePopularity( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return a->hack().info.downloads < b->hack().info.downloads;
}

static bool compareRating( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return getRating( a->hack() ) < getRating( b->hack() );
}

static bool compareDifficulty( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return getDifficulty( a->hack() ) < getDifficulty( b->hack() );
}

static bool compareLastPlayed( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return a->hack().lastPlayed < b->hack().lastPlayed;
}

static bool comparePlayTime( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return a->hack().playTime < b->hack().playTime;
}

static bool compareStarCount( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return a->hack().info.starCount < b->hack().info.starCount;
}

static bool compareFixedShuffle( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return a->shuffleOrderG() < b->shuffleOrderG();
}

static bool compareRandomShuffle( const RhdcHackWidget *a, const RhdcHackWidget *b ) {
	return a->shuffleOrderS() < b->shuffleOrderS();
}

static inline uint64 generateNewSeed() {
	uint64 seed;
	RNG::getBytes( &seed, sizeof( uint64 ) );
	seed *= 14313749767032793493ull;
	seed ^= seed >> 47;
	seed *= 14313749767032793493ull;
	return seed;
}

static bool (*s_hackComparators[])(const RhdcHackWidget*,const RhdcHackWidget*) = {
	compareName,
	comparePopularity,
	compareRating,
	compareDifficulty,
	compareLastPlayed,
	comparePlayTime,
	compareStarCount,
	compareFixedShuffle,
	compareRandomShuffle
};

RhdcHackRenderer::RhdcHackRenderer( QObject *parent ) : QStyledItemDelegate( parent ) {}
RhdcHackRenderer::~RhdcHackRenderer() {}

QWidget *RhdcHackRenderer::createEditor( QWidget *parent, const QStyleOptionViewItem&, const QModelIndex &index ) const {
	const RhdcHackListView *view = dynamic_cast<const RhdcHackListView*>( this->parent() );
	const RhdcHackWidget *widget = static_cast<const RhdcHackWidget*>( index.internalPointer() );

	if( widget == nullptr ) return new QWidget();
	RhdcHackWidget *activeWidget = widget->clone( parent, true );
	activeWidget->setFixedWidth( view->width() - view->verticalScrollBar()->width() - 6 );

	connect( view, &RhdcHackListView::resized, activeWidget, &RhdcHackWidget::editorResized );
	return activeWidget;
}

void RhdcHackRenderer::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	const RhdcHackListView *view = static_cast<const RhdcHackListView*>( parent() );
	RhdcHackWidget *widget = static_cast<RhdcHackWidget*>( index.internalPointer() );
	if( widget == nullptr ) return;

	QStyleOptionViewItem preparedOption = option;
	initStyleOption( &preparedOption, index );

	const bool selected = (view->currentIndex() == index);
	if( selected ) {
		widget = widget->clone( nullptr, true );
	}

	const QPoint rtlFix = (view->layoutDirection() == Qt::RightToLeft) ? QPoint( view->verticalScrollBar()->width(), 0 ) : QPoint( 0, 0 );

	widget->setFixedWidth( view->width() - view->verticalScrollBar()->width() - 6 );
	widget->render( painter,view->mapTo( view->window(), view->contentsRect().topLeft() + preparedOption.rect.topLeft() + rtlFix ) );

	if( selected ) widget->deleteLater();
}

QSize RhdcHackRenderer::sizeHint( const QStyleOptionViewItem&, const QModelIndex& ) const {
	const RhdcHackListView *view = dynamic_cast<const RhdcHackListView*>( parent() );
	if( view->width() < 550 ) {
		return QSize( 550, 129 );
	} else {
		return QSize( view->width(), 129 );
	}
}

RhdcHackListView::RhdcHackListView( QWidget *parent ) : QListView( parent ) {
	setModel( new RhdcHackListModel( this ) );
	setUniformItemSizes( true );
	setFlow( QListView::TopToBottom );
	setLayoutMode( QListView::Batched );
	setBatchSize( 10 );
	setItemDelegate( new RhdcHackRenderer( this ) );
	setEditTriggers( EditTrigger::CurrentChanged );
	setVerticalScrollMode( ScrollMode::ScrollPerPixel );
	verticalScrollBar()->setSingleStep( 10 );
	verticalScrollBar()->setPageStep( 129 );

	connect( selectionModel(), SIGNAL( currentChanged(const QModelIndex&,const QModelIndex&) ), this, SLOT( modelIndexChanged(const QModelIndex&,const QModelIndex&) ) );
}

RhdcHackListView::~RhdcHackListView() {
	itemDelegate()->deleteLater();
	model()->deleteLater();
}

void RhdcHackListView::modelIndexChanged( const QModelIndex &index, const QModelIndex& ) {
	if( index.isValid() && index.internalPointer() ) {
		emit selectedHackChanged( &static_cast<const RhdcHackWidget*>( index.internalPointer() )->hack() );
	} else {
		emit selectedHackChanged( nullptr );
	}
}

void RhdcHackListView::searchChanged( QString search ) {
	static_cast<RhdcHackListModel*>( model() )->setSearch( search );
}

void RhdcHackListView::focusInEvent( QFocusEvent *event ) {
	if( currentIndex().isValid() ) {
		edit( currentIndex() );
	}
	QListView::focusInEvent( event );
}

void RhdcHackListView::setPlaylist( const string &playlist ) {
	const RhdcHackExt *selectedHack = currentHack();
	const string selectedHackId = selectedHack ? selectedHack->info.hackId : "";

	scrollToTop();
	static_cast<RhdcHackListModel*>( model() )->setPlaylist( playlist );

	selectHack( selectedHackId );
}

void RhdcHackListView::reloadAllHacks( bool reshuffle ) {
	if( reshuffle ) static_cast<RhdcHackListModel*>( model() )->prepReshuffle();

	const RhdcHackExt *selectedHack = currentHack();
	const string selectedHackId = selectedHack ? selectedHack->info.hackId : "";
	const int scrollY = verticalScrollBar()->value();
	static_cast<RhdcHackListModel*>( model() )->reloadAll();
	selectHack( selectedHackId );
	verticalScrollBar()->setRange( 0, model()->rowCount( QModelIndex() ) * 129 );
	verticalScrollBar()->setValue( scrollY );

	//FIXME: Dumb hack. Need to figure out what's causing the scrolling to reset in some cases
	QtThread::safeAsync( [this,scrollY](){
		verticalScrollBar()->setRange( 0, this->model()->rowCount( QModelIndex() ) * 129 );
		this->verticalScrollBar()->setValue( scrollY );
	});
}

void RhdcHackListView::reloadHack( const string &hackId ) {
	const RhdcHackExt *selectedHack = currentHack();
	const string selectedHackId = selectedHack ? selectedHack->info.hackId : "";
	static_cast<RhdcHackListModel*>( model() )->reloadOne( hackId );
	if( !selectedHackId.empty() ) selectHack( selectedHackId );
}

void RhdcHackListView::setSortOrder( RhdcSorting sortBy, bool ascending ) {
	const RhdcHackExt *selectedHack = currentHack();
	const string selectedHackId = selectedHack ? selectedHack->info.hackId : "";
	const int scrollY = verticalScrollBar()->value();

	static_cast<RhdcHackListModel*>( model() )->setSortOrder( sortBy, ascending );

	selectHack( selectedHackId );
	verticalScrollBar()->setValue( scrollY );
}

const RhdcHackExt *RhdcHackListView::currentHack() const {
	const QModelIndex i = currentIndex();
	if( i.isValid() && i.internalPointer() ) {
		return &static_cast<const RhdcHackWidget*>( i.internalPointer() )->hack();
	}
	return nullptr;
}

void RhdcHackListView::selectHack( const string &hackId ) {
	const QModelIndex index = static_cast<RhdcHackListModel*>( model() )->findHack( hackId );
	selectionModel()->setCurrentIndex( index, QItemSelectionModel::ClearAndSelect );
	if( index.isValid() ) scrollTo( index, QAbstractItemView::EnsureVisible );
	emit selectedHackChanged( currentHack() );
}

void RhdcHackListView::resizeEvent( QResizeEvent *event ) {
	QListView::resizeEvent( event );
	emit resized( width() - verticalScrollBar()->width() - 6 );
}

RhdcHackListModel::RhdcHackListModel( QObject *parent ) : QAbstractItemModel( parent ) {
	const RhdcUiState &uiState = FileController::loadUiState().rhdcList;
	m_sortBy = uiState.sortBy;
	m_sortAsc = uiState.sortAsc;
	m_playlist = uiState.playlist;
	m_shuffleSeed = generateNewSeed();
}

RhdcHackListModel::~RhdcHackListModel() {}

void RhdcHackListModel::reloadOne( const string &hackId ) {
	const auto row = m_indexMap.find( hackId );
	if( row == m_indexMap.end() || row->second >= (int)m_hacks.size() ) return;

	const QModelIndex hackIndex = createIndex( row->second, 0, (void*)m_hacks[row->second] );
	emit dataChanged( hackIndex, hackIndex );
}

void RhdcHackListModel::setSearch( const QString &search ) {
	m_search = search.toStdString();
	reloadAll();
}

void RhdcHackListModel::setPlaylist( const string &playlist ) {
	if( m_playlist == playlist ) return;
	m_playlist = playlist;
	reloadAll();
}

void RhdcHackListModel::reloadAll() {
	beginResetModel();

	for( RhdcHackWidget *hack : m_hacks ) delete hack;
	m_hacks.clear();

	const std::vector<RhdcHackExt> hacks = m_playlist.empty() ?
		DataProvider::fetchExtRhdcHacks( m_search ) :
		DataProvider::fetchExtRhdcHacksInList( m_playlist, m_search );

	m_hacks.reserve( hacks.size() );
	for( const RhdcHackExt &hack : hacks ) {
		RhdcHackWidget *hackWidget = new RhdcHackWidget( nullptr, hack, m_shuffleSeed );
		m_hacks.push_back( hackWidget );
	}

	doSort();
	endResetModel();
}

QVariant RhdcHackListModel::data( const QModelIndex&, int ) const {
	return QVariant();
}

void RhdcHackListModel::prepReshuffle() {
	m_shuffleSeed = generateNewSeed();
	for( RhdcHackWidget *hackWidget : m_hacks ) {
		hackWidget->reshuffle( m_shuffleSeed );
	}
}

void RhdcHackListModel::doSort() {
	m_indexMap.clear();
	if( m_sortAsc ) {
		std::sort( m_hacks.begin(), m_hacks.end(), s_hackComparators[(int)m_sortBy] );
	} else {
		std::sort( m_hacks.rbegin(), m_hacks.rend(), s_hackComparators[(int)m_sortBy] );
	}

	int i = 0;
	for( const RhdcHackWidget *widget : m_hacks ) {
		m_indexMap[widget->hack().info.hackId] = i++;
	}
}

QModelIndex RhdcHackListModel::findHack( const string &hackId ) const {
	const auto row = m_indexMap.find( hackId );
	if( row == m_indexMap.end() || row->second >= (int)m_hacks.size() ) {
		return QModelIndex();
	}

	return createIndex( row->second, 0, (void*)m_hacks[row->second] );
}

Qt::ItemFlags RhdcHackListModel::flags( const QModelIndex &index ) const {
	if( !index.isValid() ) return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

QModelIndex RhdcHackListModel::index( int row, int column, const QModelIndex &parent ) const {
	if( column != 0 || parent.isValid() || row >= (int)m_hacks.size() ) return QModelIndex();
	return createIndex( row, 0, (void*)m_hacks[row] );
}

QModelIndex RhdcHackListModel::parent( const QModelIndex& ) const {
	return QModelIndex();
}

int RhdcHackListModel::rowCount( const QModelIndex &parent ) const {
	return parent.isValid() ? 0 : (int)m_hacks.size();
}

int RhdcHackListModel::columnCount( const QModelIndex& ) const {
	return 1;
}

void RhdcHackListModel::setSortOrder( RhdcSorting sortBy, bool ascending ) {
	m_sortBy = sortBy;
	m_sortAsc = ascending;
	if( sortBy == RhdcSorting::RandomShuffle ) prepReshuffle();
	beginResetModel();
	doSort();
	endResetModel();
}

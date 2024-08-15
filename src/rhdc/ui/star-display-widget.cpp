#include "src/rhdc/ui/star-display-widget.hpp"

#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QByteArray>
#include <QScrollBar>
#include <QImage>
#include <QLabel>
#include <QFile>
#include <QIcon>
#include "src/polyfill/file.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/steam-deck.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/buffer.hpp"

static QPixmap pixmapFromData( const std::vector<ubyte> &data ) {
	if( data.empty() ) return QPixmap();
	const QByteArray imageData = QByteArray::fromRawData( (char*)data.data(), (int)data.size() );
	return QPixmap::fromImage( QImage::fromData( imageData ) );
}

StarDisplayWidget::StarDisplayWidget(
	QWidget *parent,
	StarLayout &&layout
) :
	QTabWidget( parent ),
	m_layout( layout )
{
	setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Expanding ) );
}

void StarDisplayWidget::setup( bool editable ) {
	setTabBarAutoHide( true );
	m_starSets.clear();

	QPixmap collectedStarImage = pixmapFromData( m_layout.collectedStarIcon );
	if( collectedStarImage.isNull() ) {
		collectedStarImage = QIcon( ":/symbol/star.png" ).pixmap( 24, 24 );
	}

	QPixmap missingStarImage = pixmapFromData( m_layout.missingStarIcon );
	if( missingStarImage.isNull() ) {
		QImage shadow = collectedStarImage.toImage().convertToFormat( QImage::Format_ARGB32 );
		uint *pixels = (uint*)shadow.bits();
		for( qsizetype i = 0; i < shadow.sizeInBytes() / 4; i++ ) {
			pixels[i] = (pixels[i] & 0xFF000000u) | 0x00666666u;
		}
		missingStarImage = QPixmap::fromImage( shadow );
	}

	for( int i = 0; i < (int)m_layout.numSlots; i++ ) {
		QWidget *tab = new QWidget();
		SDWLayoutSides sides = makeColumnContainer( tab, i );

		tab->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
		if( m_layout.numSlots <= 26 ) {
			addTab( tab, ("MARIO "s + (char)('A' + i)).c_str() );
		} else {
			addTab( tab, ("MARIO "s + Number::toString( i )).c_str() );
		}

		const uint slotStart = m_layout.slotsStart + (i * m_layout.slotSize);
		const uint activeBit = m_layout.activeBit + (8 * slotStart);
		for( const StarLayout::Group &group : m_layout.groups ) {
			QVBoxLayout *uiColumn = nullptr;
			switch( group.side ) {
				case StarLayout::Side::Top: uiColumn = sides.top; break;
				case StarLayout::Side::Right: uiColumn = sides.right; break;
				case StarLayout::Side::Bottom: uiColumn = sides.bottom; break;
				default: uiColumn = sides.left; break;
			}

			QWidget *title = new QWidget();

			QHBoxLayout* titleLayout = new QHBoxLayout( title );
			titleLayout->addStretch();
			titleLayout->addWidget( new QLabel( group.name.c_str() ) );
			titleLayout->addStretch();
			titleLayout->setMargin( 0 );

			title->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
			title->setLayout( titleLayout );

			uiColumn->addWidget( title );

			QWidget *gridContainer = new QWidget();
			QGridLayout *uiGrid = new QGridLayout( gridContainer );
			int row = 0;
			for( const StarLayout::Course &course : group.courses ) {
				QLabel *label = new QLabel( course.name.c_str() );
				label->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
				uiGrid->addWidget( label, row, 0 );

				QWidget *starsWidget = new QWidget();
				starsWidget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );

				QHBoxLayout* uiStars = new QHBoxLayout( starsWidget );
				uiStars->setMargin( 0 );
				uiStars->setDirection( QBoxLayout::LeftToRight );
				uiStars->setSizeConstraint( QLayout::SetMinimumSize );
				starsWidget->setLayout( uiStars );

				for( const StarLayout::StarData &starData : course.stars ) {
					StarSetUi *starSet = new StarSetUi(
						nullptr,
						collectedStarImage,
						missingStarImage,
						editable,
						slotStart + starData.offset,
						activeBit,
						starData.mask
					);

					QWidget *cell = new QWidget();
					cell->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
					cell->setLayout( starSet );

					uiStars->addWidget( cell );
					m_starSets.push_back( starSet );
				}

				uiStars->addStretch();
				uiGrid->addWidget( starsWidget, row++, 1 );
			}

			uiGrid->setSizeConstraint( QLayout::SetMinimumSize );
			uiGrid->setMargin( 0 );

			gridContainer->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum ) );
			gridContainer->setLayout( uiGrid );

			uiColumn->addWidget( gridContainer, 0 );
		}

		sides.left->addStretch( 1 );
		sides.right->addStretch( 1 );
	}
}

void StarDisplayWidget::makeScrollable() {
	for( int i = 0; i < count(); i++ ) {
		QWidget *tab = this->widget( i );
		if( dynamic_cast<const QScrollArea*>( tab ) != nullptr ) return;

		const QString tabLabel = this->tabText( i );
		QScrollArea *scrollArea = new QScrollArea();
		scrollArea->setFrameShape( QFrame::NoFrame );
		scrollArea->setStyleSheet( "QScrollArea{ padding: 0; }" );
		scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
		scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

		this->removeTab( i );
		tab->setParent( scrollArea );
		scrollArea->setWidget( tab );
		scrollArea->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
		this->insertTab( 0, scrollArea, tabLabel );
	}
}

LiveStarDisplayWidget::LiveStarDisplayWidget(
	QWidget *parent,
	const fs::path &saveFilePath,
	StarLayout &&layout
) :
	StarDisplayWidget( parent, std::move( layout ) ),
	m_saveFilePath( saveFilePath )
{
	setup( false );
	if( !fs::existsSafe( saveFilePath ) ) {
		QFile saveFile( saveFilePath.u8string().c_str() );
		saveFile.open( QFile::WriteOnly | QFile::Truncate );

		QByteArray zeroBuffer;
		zeroBuffer.fill( '\0', 296960 );

		saveFile.write( zeroBuffer );
		saveFile.flush();
	}

	connect( &m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &LiveStarDisplayWidget::saveFileChanged );
	m_fileWatcher.addPath( saveFilePath.u8string().c_str() );
	saveFileChanged();
}

void LiveStarDisplayWidget::saveFileChanged() {
	InputFile saveFile( m_saveFilePath, true );
	Buffer saveData = m_layout.getUsedSaveData( saveFile );
	for( StarSetUi *starSet: m_starSets ) starSet->load( saveData );
}

SDWLayoutSides LiveStarDisplayWidget::makeColumnContainer( QWidget *parent, int ) {
	QWidget *columnsWidget = new QWidget();

	QVBoxLayout *rows = new QVBoxLayout( parent );
	QBoxLayout *columns;
	if( SteamDeck::isSteamDeck() ) {
		columns = new QVBoxLayout( columnsWidget );
	} else {
		columns = new QHBoxLayout( columnsWidget );
	}

	columnsWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	columnsWidget->setLayout( columns );
	columns->setSizeConstraint( QLayout::SetMinimumSize );
	columns->setMargin( 0 );

	rows->setSizeConstraint( QLayout::SetMinimumSize );

	QWidget *topContainer = new QWidget();
	QWidget *bottomContainer = new QWidget();

	QHBoxLayout *topContainerLayout = new QHBoxLayout( topContainer );
	QHBoxLayout *bottomContainerLayout = new QHBoxLayout( bottomContainer );

	topContainer->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	topContainer->setLayout( topContainerLayout );
	topContainerLayout->setSizeConstraint( QLayout::SetMinimumSize );
	topContainerLayout->setMargin( 0 );

	bottomContainer->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	bottomContainer->setLayout( bottomContainerLayout );
	bottomContainerLayout->setSizeConstraint( QLayout::SetMinimumSize );
	bottomContainerLayout->setMargin( 0 );

	QWidget *topWidget = new QWidget();
	QWidget *bottomWidget = new QWidget();
	QWidget *leftWidget = new QWidget();
	QWidget *rightWidget = new QWidget();

	QVBoxLayout *top = new QVBoxLayout( topWidget );
	QVBoxLayout *bottom = new QVBoxLayout( bottomWidget );
	QVBoxLayout *left = new QVBoxLayout( leftWidget );
	QVBoxLayout *right = new QVBoxLayout( rightWidget );

	topWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	topWidget->setLayout( top );
	top->setSizeConstraint( QLayout::SetMinimumSize );
	top->setMargin( 0 );

	bottomWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	bottomWidget->setLayout( bottom );
	bottom->setSizeConstraint( QLayout::SetMinimumSize );
	bottom->setMargin( 0 );

	leftWidget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
	leftWidget->setLayout( left );
	left->setSizeConstraint( QLayout::SetMinimumSize );
	left->setMargin( 0 );

	rightWidget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
	rightWidget->setLayout( right );
	right->setSizeConstraint( QLayout::SetMinimumSize );
	right->setMargin( 0 );

	parent->setLayout( rows );
	rows->addWidget( topContainer, 0 );
	rows->addWidget( columnsWidget, 1 );
	rows->addWidget( bottomContainer, 0 );
	topContainerLayout->addWidget( topWidget , 0 );
	topContainerLayout->addStretch( 1 );
	bottomContainerLayout->addWidget( bottomWidget, 0 );
	bottomContainerLayout->addStretch( 1 );
	columns->addWidget( leftWidget, 0 );
	columns->addStretch( 1 );
	columns->addWidget( rightWidget, 0 );

	return SDWLayoutSides{ top, right, bottom, left };
}

EditableStarDisplayWidget::EditableStarDisplayWidget(
	QWidget *parent,
	const fs::path &saveFilePath,
	StarLayout &&layout
) :
	StarDisplayWidget( parent, std::move( layout ) ),
	m_saveFilePath( saveFilePath )
{
	load();
}

SDWLayoutSides EditableStarDisplayWidget::makeColumnContainer( QWidget *parent, int slot ) {
	QVBoxLayout *page = new QVBoxLayout( parent );
	page->setSizeConstraint( QLayout::SetMinimumSize );

	QWidget *sidesWidget = new QWidget();
	QWidget *columnsWidget = new QWidget();
	QWidget *footerWidget = new QWidget();

	QHBoxLayout *footer = new QHBoxLayout( footerWidget );
	footerWidget->setLayout( footer );
	footerWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred ) );
	footer->setSizeConstraint( QLayout::SetMinimumSize );
	footer->setMargin( 0 );

	QVBoxLayout *rows = new QVBoxLayout( sidesWidget );
	QHBoxLayout *columns = new QHBoxLayout( columnsWidget );

	sidesWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	sidesWidget->setLayout( rows );
	rows->setMargin( 0 );
	rows->setSizeConstraint( QLayout::SetMinimumSize );

	columnsWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	columnsWidget->setLayout( columns );
	columns->setMargin( 0 );
	columns->setSizeConstraint( QLayout::SetMinimumSize );

	QWidget *topContainer = new QWidget();
	QWidget *bottomContainer = new QWidget();

	QHBoxLayout *topContainerLayout = new QHBoxLayout( topContainer );
	QHBoxLayout *bottomContainerLayout = new QHBoxLayout( bottomContainer );

	topContainer->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	topContainer->setLayout( topContainerLayout );
	topContainerLayout->setMargin( 0 );
	topContainerLayout->setSizeConstraint( QLayout::SetMinimumSize );

	bottomContainer->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	bottomContainer->setLayout( bottomContainerLayout );
	bottomContainerLayout->setMargin( 0 );
	bottomContainerLayout->setSizeConstraint( QLayout::SetMinimumSize );

	QWidget *topWidget = new QWidget();
	QWidget *bottomWidget = new QWidget();
	QWidget *leftWidget = new QWidget();
	QWidget *rightWidget = new QWidget();

	QVBoxLayout *top = new QVBoxLayout( topWidget );
	QVBoxLayout *bottom = new QVBoxLayout( bottomWidget );
	QVBoxLayout *left = new QVBoxLayout( leftWidget );
	QVBoxLayout *right = new QVBoxLayout( rightWidget );

	topWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	topWidget->setLayout( top );
	top->setMargin( 0 );
	top->setSizeConstraint( QLayout::SetMinimumSize );

	bottomWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	bottomWidget->setLayout( bottom );
	bottom->setMargin( 0 );
	bottom->setSizeConstraint( QLayout::SetMinimumSize );

	leftWidget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
	leftWidget->setLayout( left );
	left->setMargin( 0 );
	left->setSizeConstraint( QLayout::SetMinimumSize );

	rightWidget->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );
	rightWidget->setLayout( right );
	right->setMargin( 0 );
	right->setSizeConstraint( QLayout::SetMinimumSize );

	rows->addWidget( topContainer, 0 );
	rows->addWidget( columnsWidget, 1 );
	rows->addWidget( bottomContainer, 0 );
	topContainerLayout->addWidget( topWidget , 0 );
	topContainerLayout->addStretch( 1 );
	bottomContainerLayout->addWidget( bottomWidget, 0 );
	bottomContainerLayout->addStretch( 1 );
	columns->addWidget( leftWidget, 0 );
	columns->addStretch( 1 );
	columns->addWidget( rightWidget, 0 );

	page->addWidget( sidesWidget, 0 );
	page->addWidget( footerWidget, 0 );
	page->addStretch( 1 );

	QLabel *inactiveLabel = new QLabel( tr( "Save slot is empty." ) );
	inactiveLabel->setTextFormat( Qt::PlainText );
	inactiveLabel->setStyleSheet( "font-weight: bold;" );

	QPushButton *createButton = new QPushButton( tr( "Create Save Slot" ) );
	QPushButton *clearButton = new QPushButton( tr( "Delete Save Slot" ) );

	footer->addWidget( inactiveLabel, 0 );
	footer->addStretch( 1 );
	footer->addWidget( createButton, 0 );
	footer->addWidget( clearButton, 0 );

	sidesWidget->setVisible( m_slotActive.at( slot ) );
	clearButton->setVisible( m_slotActive.at( slot ) );
	inactiveLabel->setVisible( !m_slotActive.at( slot ) );
	createButton->setVisible( !m_slotActive.at( slot ) );

	connect( createButton, &QPushButton::clicked, [=](){
		this->m_slotActive.at( slot ) = true;
		inactiveLabel->setVisible( false );
		createButton->setVisible( false );
		clearButton->setVisible( true );
		sidesWidget->setVisible( true );
	});

	connect( clearButton, &QPushButton::clicked, [=](){
		this->m_slotActive.at( slot ) = false;
		inactiveLabel->setVisible( true );
		createButton->setVisible( true );
		clearButton->setVisible( false );
		sidesWidget->setVisible( false );
	});

	parent->setLayout( page );
	return SDWLayoutSides{ top, right, bottom, left };
}

void EditableStarDisplayWidget::save() const {
	File saveFile( m_saveFilePath, true );
	Buffer saveData = m_layout.getUsedSaveData( saveFile );

	for( StarSetUi *starSet: m_starSets ) starSet->save( saveData );
	uint byteIndex = m_layout.slotsStart + (m_layout.activeBit / 8);
	const ubyte setBit = 1 << (7 - (m_layout.activeBit % 8));
	for( ubyte i = 0; i < m_layout.numSlots; i++ ) {
		if( m_slotActive.at( i ) ) {
			saveData.data()[byteIndex] |= setBit;
		} else {
			saveData.data()[byteIndex] &= ~setBit;
		}

		if( m_layout.checksumOffset != 0 ) {
			const uint slotStart = m_layout.slotsStart + (i * m_layout.slotSize);
			const uint checksumPos = slotStart + m_layout.checksumOffset;

			ushort checksum = 0;
			for( uint j = slotStart; j < checksumPos; j++ ) {
				checksum += (ubyte)saveData.data()[j];
			}

			saveData.data()[checksumPos] = (char)(ubyte)(checksum >> 8);
			saveData.data()[checksumPos + 1] = (char)(ubyte)(checksum & 0xFF);
		}

		byteIndex += m_layout.slotSize;
	}

	m_layout.writeSaveData( saveFile, saveData );
}

void EditableStarDisplayWidget::load() {
	m_slotActive.clear();
	m_slotActive.reserve( m_layout.numSlots );

	while( this->widget( 0 ) ) {
		this->widget( 0 )->deleteLater();
		this->removeTab( 0 );
	}

	InputFile saveFile( m_saveFilePath, true );
	Buffer saveData = m_layout.getUsedSaveData( saveFile );
	saveFile.close();

	uint byteIndex = m_layout.slotsStart + (m_layout.activeBit / 8);
	const ubyte testBit = 1 << (7 - (m_layout.activeBit % 8));
	for( ubyte i = 0; i < m_layout.numSlots; i++ ) {
		m_slotActive.push_back( saveData.data()[byteIndex] & testBit );
		byteIndex += m_layout.slotSize;
	}

	setup( true );

	for( StarSetUi *starSet: m_starSets ) starSet->load( saveData );
}

#include "src/ui/flow-layout.hpp"

#include <QGuiApplication>
#include <QWidget>
#include <QStyle>

FlowLayout::FlowLayout( QWidget *parent ) : QLayout( parent ) {}
FlowLayout::FlowLayout( QWidget *parent, QSize &&spacing ) : QLayout( parent ), m_spacing( spacing ) {}

FlowLayout::~FlowLayout() {
	while( QLayoutItem *item = takeAt( 0 ) ) {
		delete item;
	}
}

int FlowLayout::doLayout( const QRect &rect, bool test ) const {
	const QMargins margins = contentsMargins();
	const QSize spacing = getItemSpacing();

	const QRect contentRegion = rect.adjusted( margins.left(), margins.top(), -margins.right(), -margins.bottom() );
	const bool rtl = QGuiApplication::layoutDirection() == Qt::RightToLeft;

	int x = contentRegion.x();
	int y = contentRegion.y();
	int lineHeight = 0;

	for( QLayoutItem *item : m_items ) {
		if( item->isEmpty() ) continue;

		int spaceWidth = spacing.width();
		if( spaceWidth < 0 ) {
			spaceWidth = item->widget()->style()->layoutSpacing( QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal );
		}

		int spaceHeight = spacing.height();
		if( spaceHeight < 0 ) {
			spaceHeight = item->widget()->style()->layoutSpacing( QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical );
		}

		int nextX = x + item->sizeHint().width() + spaceWidth;
		if( nextX - spaceWidth > contentRegion.right() && lineHeight > 0 ) {
			nextX += contentRegion.x() - x;
			x = contentRegion.x();
			y += lineHeight + spaceHeight;
			lineHeight = 0;
		}

		if( !test ) {
			const int realX = rtl ? (rect.right() - nextX + spaceWidth ) : x;
			item->setGeometry( QRect( QPoint( realX, y ), item->sizeHint() ) );
		}

		x = nextX;
		lineHeight = qMax( lineHeight, item->sizeHint().height() );
	}

	return y + lineHeight - rect.y() + margins.bottom();

}

QSize FlowLayout::getItemSpacing() const {
	if( m_spacing.isValid() ) return m_spacing;
	if( parent() == nullptr ) return QSize();

	if( parent()->isWidgetType() ) {
		const QWidget *parentWidget = static_cast<const QWidget*>( parent() );
		return QSize(
			m_spacing.width() >= 0 ? m_spacing.width() : parentWidget->style()->pixelMetric( QStyle::PM_LayoutHorizontalSpacing, nullptr, parentWidget ),
			m_spacing.height() >= 0 ? m_spacing.height() : parentWidget->style()->pixelMetric( QStyle::PM_LayoutVerticalSpacing, nullptr, parentWidget )
		);
	} else {
		const QLayout *parentLayout = static_cast<const QLayout*>( parent() );
		return QSize(
			m_spacing.width() >= 0 ? m_spacing.width() : parentLayout->spacing(),
			m_spacing.height() >= 0 ? m_spacing.height() : parentLayout->spacing()
		);
	}
}

void FlowLayout::setItemSpacing( QSize &&spacing ) {
	m_spacing = spacing;
	doLayout( geometry(), false );
}

Qt::Orientations FlowLayout::expandingDirections() const {
	return {};
}

bool FlowLayout::hasHeightForWidth() const {
	return true;
}

int FlowLayout::heightForWidth( int width ) const {
	return doLayout( QRect( 0, 0, width, 0 ), true );
}

int FlowLayout::count() const {
	return m_items.size();
}

QLayoutItem *FlowLayout::itemAt( int index ) const {
	return m_items.value( index );
}

QSize FlowLayout::minimumSize() const {
	QSize size( 0, 0 );
	for( const QLayoutItem *item : m_items ) {
		size = size.expandedTo( item->minimumSize() );
	}

	return size + QSize( 2 * margin(), 2 * margin() );
}

QSize FlowLayout::sizeHint() const {
	return minimumSize();
}

void FlowLayout::addItem( QLayoutItem *item ) {
	m_items.append( item );
}

void FlowLayout::setGeometry( const QRect &rect ) {
	QLayout::setGeometry( rect );
	doLayout( rect, false );
}

QLayoutItem *FlowLayout::takeAt( int index ) {
	if( index < 0 || index >= m_items.size() ) {
		return nullptr;
	}

	return m_items.takeAt( index );
}

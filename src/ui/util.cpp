#include "src/ui/util.hpp"

#include <QDesktopWidget>
#include <QApplication>
#include <QWidget>
#ifdef __APPLE__
#include <QFont>
#endif
#include <algorithm>

static void freeLayoutItem( QLayoutItem *item ) {
	if( item->layout() ) {
		UiUtil::clearLayout( item->layout() );
		item->layout()->deleteLater();
	}

	if( item->widget() ) {
		item->widget()->deleteLater();
	}

	delete item;
}

void UiUtil::clearLayout( QLayout *layout ) {
	while( QLayoutItem *item = layout->takeAt( 0 ) ) {
		freeLayoutItem( item );
	}
}

void UiUtil::shrinkToFit( QDialog *dialog, const QFlags<Qt::Orientation> &shrinkDirection ) {
	QRect geo = dialog->geometry();
	if( shrinkDirection & Qt::Horizontal ) {
		geo.setWidth( 0 );
	}
	if( shrinkDirection & Qt::Vertical ) {
		geo.setHeight( 0 );
	}
	dialog->setGeometry( geo );
}

#ifdef __APPLE__
void UiUtil::fixFontSizeOnMac( QLabel *label ) {
	if( label->font().pointSize() > 0 ) {
		QFont font = label->font();
		font.setPointSizeF( (qreal)font.pointSize() * 4.0 / 3.0 );
		label->setFont( font );
	}
}

static constexpr int BASE_DPI = 72;
#else
static constexpr int BASE_DPI = 96;
#endif

void UiUtil::setIconSize( QAbstractButton *button, int baseIconSize ) {
	static const int dpi = std::max({ BASE_DPI, QApplication::desktop()->logicalDpiX(), QApplication::desktop()->logicalDpiY() });
	const int scaledIconSize = baseIconSize * dpi / BASE_DPI;
	button->setIconSize( QSize( scaledIconSize, scaledIconSize ) );
}

void UiUtil::scaleWindow( QWidget *window ) {
	const double scale = (double)QApplication::desktop()->logicalDpiY() / (double)BASE_DPI;
	if( scale > 1.0 ) window->resize( window->size() * scale );
}

#ifdef _WIN32
void UiUtil::fixDialogButtonsOnWindows( QDialog *dialog ) {
	dialog->setWindowFlag( Qt::CustomizeWindowHint, true );
	dialog->setWindowFlag( Qt::WindowTitleHint, true );
	dialog->setWindowFlag( Qt::WindowMaximizeButtonHint, true );
	dialog->setWindowFlag( Qt::WindowCloseButtonHint, true );
}
#endif

#include "src/ui/ui-fixes.hpp"

#include <QLabel>
#include "src/ui/tooltip-hack.hpp"
#include "src/ui/palette.hpp"
#include "src/ui/icons.hpp"
#include "src/core/steam-deck.hpp"

#ifndef __linux__
#include <QLibrary>
#include <QPluginLoader>
#include <QTextStream>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QIcon>
#include <QFont>
#include <QPalette>
#include <QColor>
#include <iostream>
#include <algorithm>
#include "src/core/file-controller.hpp"
#include "src/core/numeric-string.hpp"
#include "src/polyfill/base-directory.hpp"

#ifdef __APPLE__
#include <QFontDatabase>
#endif

static constexpr QMessageBox::StandardButtons s_positiveButtons =
	QMessageBox::Ok | QMessageBox::Apply | QMessageBox::Yes | QMessageBox::YesToAll;

static constexpr QMessageBox::StandardButtons s_negativeButtons =
	QMessageBox::Cancel | QMessageBox::Abort | QMessageBox::No | QMessageBox::NoToAll | QMessageBox::Close;

bool WindowsIconFixer::eventFilter( QObject *object, QEvent *event ) {
	if( event->type() != QEvent::Show ) {
		return QObject::eventFilter( object, event );
	}

	QMessageBox *dialog = dynamic_cast<QMessageBox*>( object );
	if( dialog != nullptr ) {
		for( QAbstractButton *dialogButton : dialog->buttons() ) {
			const QMessageBox::StandardButtons buttonType = dialog->standardButton( dialogButton );
			if( buttonType & s_positiveButtons ) {
				dialogButton->setIcon( Icon::ok() );
			} else if( buttonType & s_negativeButtons ) {
				dialogButton->setIcon( Icon::cancel() );
			} else if( buttonType & (QMessageBox::Save | QMessageBox::SaveAll) ) {
				dialogButton->setIcon( Icon::save() );
			} else if( buttonType & QMessageBox::Open ) {
				dialogButton->setIcon( Icon::browse() );
			}
		}
	}

	QDialogButtonBox *buttonTray = dynamic_cast<QDialogButtonBox*>( object );
	if( buttonTray != nullptr ) {
		for( QAbstractButton *dialogButton : buttonTray->buttons() ) {
			const QDialogButtonBox::StandardButton buttonType = buttonTray->standardButton( dialogButton );
			switch( buttonType ) {
				case QDialogButtonBox::StandardButton::Ok:
				case QDialogButtonBox::StandardButton::Apply:
				case QDialogButtonBox::StandardButton::Yes:
				case QDialogButtonBox::StandardButton::YesToAll:
					dialogButton->setIcon( Icon::ok() ); break;
				case QDialogButtonBox::StandardButton::Open:
					dialogButton->setIcon( Icon::browse() ); break;
				case QDialogButtonBox::StandardButton::Save:
				case QDialogButtonBox::StandardButton::SaveAll:
					dialogButton->setIcon( Icon::save() ); break;
				case QDialogButtonBox::StandardButton::Cancel:
					dialogButton->setIcon( Icon::cancel() ); break;
				case QDialogButtonBox::StandardButton::Close:
				case QDialogButtonBox::StandardButton::No:
				case QDialogButtonBox::StandardButton::NoToAll:
				case QDialogButtonBox::StandardButton::Abort:
					dialogButton->setIcon( Icon::close() ); break;
				case QDialogButtonBox::StandardButton::Reset:
				case QDialogButtonBox::StandardButton::Retry:
					dialogButton->setIcon( Icon::refresh() ); break;
				default: break;
			}
		}
	}

	return QObject::eventFilter( object, event );
};

static const char *s_cssTweaks = R"THEME_DEF(

QPushButton, QToolButton {
	padding: 7px;
}

QComboBox {
	padding: 5px;
}

QSpinBox, QDoubleSpinBox {
	padding: 4px 2px;
}

QLineEdit {
	padding: 4px;
}

QListWidget::item, QTreeView::item {
	padding: 4px 1px;
}

QMessageBox QAbstractButton, QDialogButtonBox QAbstractButton {
	min-width: 66px;
}

QGroupBox::title {
	subcontrol-position: top center;
}

)THEME_DEF";

static inline void applyWindowsFixes( QApplication &app ) {
#ifdef __APPLE__
	fs::directory_iterator pluginDir( BaseDir::program().parent_path() / "PlugIns" / "styles" );
#else
	fs::directory_iterator pluginDir( BaseDir::program() / L"styles" );
#endif
	for( const auto &plugin : pluginDir ) {
		const QString pluginPath = plugin.path().u8string().c_str();
		if( !QLibrary::isLibrary( pluginPath ) ) continue;

		QPluginLoader *pluginLoader = new QPluginLoader( pluginPath, &app );
		if( !pluginLoader->load() ) {
			std::cerr << "Failed to load style plugin " << plugin.path().filename().u8string() << std::endl << std::flush;
			pluginLoader->deleteLater();
		}
	}

#ifdef __APPLE__
	constexpr int BASE_DPI = 72;
#else
	constexpr int BASE_DPI = 96;
#endif

	string cssTweaks = s_cssTweaks;
	const int dpi = std::max({ BASE_DPI, QApplication::desktop()->logicalDpiX(), QApplication::desktop()->logicalDpiY() });
	const string defaultIconSize = Number::toString( 16 * dpi / BASE_DPI ) + "px";
	const string largeIconSize = Number::toString( 24 * dpi / BASE_DPI ) + "px";

	cssTweaks += "QPushButton { qproperty-iconSize: "s + defaultIconSize + ' ' + defaultIconSize + "; }\n";
	cssTweaks += string(
		"QMainWindow#MainWindow QPushButton#refreshButton, "
		"QMainWindow#MainWindow QPushButton#controllerConfigButton, "
		"QMainWindow#MainWindow QPushButton#rhdcViewToggleButton, "
		"QMainWindow#MainWindow QToolButton#menuButton { "
			"qproperty-iconSize: "
	) + largeIconSize + ' ' + largeIconSize + "; }\n";

	AppSettings settings = FileController::loadAppSettings();
	if( app.setStyle( settings.theme.c_str() ) == nullptr ) {
		app.setStyle( "Fusion" );
		settings.theme = "Fusion";
		FileController::saveAppSettings( settings );
	}

	app.setStyleSheet( cssTweaks.c_str() );
	QIcon::setThemeName( "breeze-fallback" );

#ifdef __APPLE__
	const fs::path fontDir = BaseDir::program().parent_path() / "Resources" / "fonts";
	const int fontId = QFontDatabase::addApplicationFont( (fontDir / "NotoSans-Regular.ttf").c_str() );
	QFontDatabase::addApplicationFont( (fontDir / "NotoSans-Bold.ttf").c_str() );
	QFontDatabase::addApplicationFont( (fontDir / "NotoSans-Italic.ttf").c_str() );
	QFontDatabase::addApplicationFont( (fontDir / "NotoSans-BoldItalic.ttf").c_str() );
	QFontDatabase::addApplicationFont( (fontDir / "NotoMono-Regular.ttf").c_str() );
	QFont defaultFont = (fontId >= 0) ? QFont( QFontDatabase::applicationFontFamilies( fontId ).at( 0 ) ) : QFont( "Noto Sans" );
#else
	QFont defaultFont( "Noto Sans" );
#endif

	defaultFont.setStyleHint( QFont::SansSerif );
#ifdef __APPLE__
	defaultFont.setPointSize( 13 );
#else
	defaultFont.setPointSize( 10 );
#endif
	defaultFont.setKerning( true );
	app.setFont( defaultFont );

	const bool darkMode = FileController::loadAppSettings().darkMode;
	app.setPalette( darkMode ? ColourPalette::dark() : ColourPalette::light() );

	app.installEventFilter( new WindowsIconFixer( &app ) );
}

#undef PALETTE_DEF1
#undef PALETTE_DEF2
#undef PALETTE_DEF3

#else
static inline void applyWindowsFixes( [[maybe_unused]] const QApplication &app ) {}
#endif

bool RtlLabelFixer::eventFilter( QObject *object, QEvent *event ) {
	if( event->type() != QEvent::Show ) {
		return QObject::eventFilter( object, event );
	}

	QLabel *label = dynamic_cast<QLabel*>( object );
	if( label != nullptr && label->layoutDirection() == Qt::RightToLeft ) {
		switch( label->alignment() & Qt::AlignHorizontal_Mask ) {
			case Qt::AlignLeft:
			case Qt::AlignJustify:
				label->setAlignment( (label->alignment() & Qt::AlignVertical_Mask) | Qt::AlignRight );
				break;
			default:
				break;
		}
	}

	return QObject::eventFilter( object, event );
};

void applyUiFixes( QApplication &app ) {
	app.setWindowIcon( Icon::appIcon() );
	app.installEventFilter( new TooltipHack( &app ) );
	app.installEventFilter( new RtlLabelFixer( &app ) );
	applyWindowsFixes( app );

	if( SteamDeck::inGamingMode() ) {
		app.setStyle( ColourPalette::systemIsDarkMode() ? "Breeze Dark" : "Breeze" );
	}
}

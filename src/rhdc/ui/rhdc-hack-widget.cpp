#include "src/rhdc/ui/rhdc-hack-widget.hpp"
#include "ui_rhdc-hack.h"

#include <QDateTime>
#include <QLocale>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <unordered_map>
#include "src/polyfill/base-directory.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/uuid.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include "src/rhdc/web/api.hpp"

static HashMap<string, ThumbnailNotifier*> s_thumbnailDownloaders;

void ThumbnailNotifier::notifyDownloaded() {
	emit downloaded();
}

static QString formatRating( double avgRating, ubyte userRating, bool kaizo ) {
	if( avgRating <= 0.0 ) {
		if( userRating != 0 ) {
			return QString( "%L1*" ).arg( (int)userRating + (kaizo ? 10 : 0) );
		} else {
			return QString( "-" );
		}
	} else if( userRating != 0 ) {
		return QString( "%L1 (%L2*)" ).arg( avgRating + (kaizo ? 10.0 : 0.0), 0, 'f', 1 ).arg( (int)userRating + (kaizo ? 10 : 0) );
	} else {
		return QString( "%L1" ).arg( avgRating + (kaizo ? 10.0 : 0.0), 0, 'f', 1 );
	}
}

static constexpr int64 SECONDS = 1000;
static constexpr int64 MINUTES = 60 * SECONDS;
static constexpr int64 HOURS = 60 * MINUTES;

static inline string formatTimeSpan( int64 timeSpan ) {
	const int64 hours = timeSpan / HOURS;
	const int64 minutes = (timeSpan % HOURS) / MINUTES;
	const int64 seconds = (timeSpan % MINUTES) / SECONDS;

	string timeString;
	timeString.reserve( 16 );

	if( hours > 0 ) {
		timeString += Number::toString( hours );
		timeString += "h ";
	}

	if( minutes > 0 || hours > 0 ) {
		timeString += Number::toString( minutes );
		timeString += "m ";
	}

	timeString += Number::toString( seconds );
	timeString += 's';

	return timeString;
}

struct IconSet {
	QPixmap download;
	QPixmap rating;
	QPixmap difficulty;
	QPixmap lastPlayed;
	QPixmap playTime;
};

static QPixmap loadColouredIcon( const QString &path, uint colour ) {
	QImage icon = QIcon( path ).pixmap( 16, 16 ).toImage().convertToFormat( QImage::Format_ARGB32 );
	uint *pixels = (uint*)icon.bits();
	for( qsizetype i = 0; i < icon.sizeInBytes() / 4; i++ ) {
		pixels[i] = (pixels[i] & 0xFF000000) | (colour & 0x00FFFFFF);
	}
	return QPixmap::fromImage( icon );
}

static IconSet getIconSet( const QColor &colour ) {
	return IconSet{
		loadColouredIcon( ":/symbol/download.svg", (uint)colour.rgb() ),
		loadColouredIcon( ":/symbol/rating.svg", (uint)colour.rgb() ),
		loadColouredIcon( ":/symbol/difficulty.svg", (uint)colour.rgb() ),
		loadColouredIcon( ":/symbol/last-played.svg", (uint)colour.rgb() ),
		loadColouredIcon( ":/symbol/play-time.svg", (uint)colour.rgb() )
	};
}

static const IconSet &getIconSetNormal( QWidget *widget ) {
	static const IconSet s_iconSet = getIconSet( widget->palette().text().color() );
	return s_iconSet;
}

static const IconSet &getIconSetSelected( QWidget *widget ) {
	static const IconSet s_iconSet = getIconSet( widget->palette().highlightedText().color() );
	return s_iconSet;
}

static inline uint64 hashString( const string &str ) noexcept {
	uint64 hash = 14695981039346656037ull;
	for( char c : str ) {
		hash ^= (uint64)(ubyte)c;
		hash *= 1099511628211ull;
	}
	return hash;
}

RhdcHackWidget::RhdcHackWidget(
	QWidget *parent,
	const RhdcHackExt &hack,
	uint64 shuffleSeed
) :
	QWidget( parent ),
	m_ui( new Ui::RhdcHackWidget ),
	m_hack( hack ),
	m_shuffleOrder( hashString( hack.info.hackId ) ),
	m_thumbnailLoaded( false )
{
	reshuffle( shuffleSeed );
	initialize( false );
}

RhdcHackWidget::RhdcHackWidget(
	QWidget *parent,
	const RhdcHackExt &hack,
	bool selected,
	uint64 shuffleOrderG,
	uint64 shuffleOrderS
) :
	QWidget( parent ),
	m_ui( new Ui::RhdcHackWidget ),
	m_hack( hack ),
	m_shuffleOrder( shuffleOrderG ),
	m_seededShuffleOrder( shuffleOrderS ),
	m_thumbnailLoaded( false )
{
	initialize( selected );
}

RhdcHackWidget::~RhdcHackWidget() {
	delete m_ui;
}

void RhdcHackWidget::initialize( bool selected ) {
	m_ui->setupUi( this );
	setAutoFillBackground( true );

	setMinimumHeight( 129 );
	setMaximumHeight( 129 );

	if( layoutDirection() == Qt::RightToLeft ) {
		m_ui->titleLabel->setAlignment( Qt::AlignRight );
		m_ui->starCountLabel->setAlignment( Qt::AlignRight );
		m_ui->downloadsLabel->setAlignment( Qt::AlignRight );
		m_ui->ratingLabel->setAlignment( Qt::AlignRight );
		m_ui->difficultyLabel->setAlignment( Qt::AlignRight );
		m_ui->lastPlayedLabel->setAlignment( Qt::AlignRight );
		m_ui->playTimeLabel->setAlignment( Qt::AlignRight );
	}

#ifdef __APPLE__
	UiUtil::fixFontSizeOnMac( m_ui->titleLabel );
	m_ui->verticalLayout->setSpacing( 0 );
#endif

	m_ui->titleLabel->setText( m_hack.info.name.c_str() );
	m_ui->description->setPlainText( m_hack.info.description.c_str() );
	m_ui->description->setAutoFillBackground( false );

	m_ui->downloadsLabel->setText( QLocale().toString( m_hack.info.downloads ) );
	if( m_hack.info.avgRating == 0 ) {
		m_ui->ratingLabel->setText( "-" );
	} else {
		m_ui->ratingLabel->setText( QString( "%L1" ).arg( m_hack.info.avgRating, 0, 'f', 2 ) );
	}
	m_ui->difficultyLabel->setText( formatRating( m_hack.info.avgDifficulty, m_hack.progress.difficulty, m_hack.info.category == "Kaizo" ) );
	if( m_hack.lastPlayed > 0 ) {
		m_ui->lastPlayedLabel->setText( QDateTime::fromMSecsSinceEpoch( m_hack.lastPlayed ).toString( "yyyy-MM-dd h:mm AP" ) );
	} else {
		/*: Text shown in place of a date when a hack hasn't been played yet */
		m_ui->lastPlayedLabel->setText( tr( "Never" ) );
	}
	m_ui->playTimeLabel->setText( formatTimeSpan( m_hack.playTime ).c_str() );

	if( m_hack.info.starCount > 0 ) {
		m_ui->progressWidget->setCurrentIndex( 0 );
		if( layoutDirection() == Qt::RightToLeft ) {
			m_ui->starCountLabel->setText( QString( " %L2 \\ %L1" ).arg( m_hack.progress.stars ).arg( m_hack.info.starCount ) );
		} else {
			m_ui->starCountLabel->setText( QString( "%L1 / %L2 " ).arg( m_hack.progress.stars ).arg( m_hack.info.starCount ) );
		}
	} else {
		m_ui->progressWidget->setCurrentIndex( 1 );
		m_ui->completionLabel->setText( m_hack.progress.completed ? tr( "Completed" ) : tr( "Incomplete" ) );

		QPalette textColour = m_ui->completionLabel->palette();
		textColour.setColor( QPalette::WindowText, m_hack.progress.completed ? Qt::darkGreen : Qt::red );
		m_ui->completionLabel->setPalette( textColour );
	}

	const IconSet &iconSet = selected ? getIconSetSelected( this ) :getIconSetNormal( this );
	m_ui->downloadIcon->setPixmap( iconSet.download );
	m_ui->ratingIcon->setPixmap( iconSet.rating );
	m_ui->difficultyIcon->setPixmap( iconSet.difficulty );
	m_ui->lastPlayedIcon->setPixmap( iconSet.lastPlayed );
	m_ui->playTimeIcon->setPixmap( iconSet.playTime );

	QPalette p = palette();
	if( selected ) {
		p.setBrush( QPalette::Window, p.highlight() );
		p.setBrush( QPalette::Base, p.highlight() );
		p.setBrush( QPalette::WindowText, p.highlightedText() );
		p.setBrush( QPalette::ButtonText, p.highlightedText() );
		p.setBrush( QPalette::Text, p.highlightedText() );
		m_ui->description->setPalette( p );

#ifndef __linux__
		m_ui->titleLabel->setPalette( p );
		m_ui->starCountLabel->setPalette( p );
		m_ui->downloadsLabel->setPalette( p );
		m_ui->ratingLabel->setPalette( p );
		m_ui->difficultyLabel->setPalette( p );
		m_ui->lastPlayedLabel->setPalette( p );
		m_ui->playTimeLabel->setPalette( p );
#endif
	} else {
		p.setBrush( QPalette::Window, p.base() );
	}
	setPalette( p );
}

static inline void scaleAndZoom( QPixmap &pixmap, const QSize &size ) {
	if( pixmap.size() == size ) return;

	pixmap = pixmap.scaled( size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
	if( pixmap.size() == size ) return;

	QRect zoomRegion = pixmap.rect();

	if( pixmap.width() > size.width() ) {
		zoomRegion.setX( (zoomRegion.width() - size.width()) / 2 );
		zoomRegion.setWidth( size.width() );
	}

	if( pixmap.height() > size.height() ) {
		zoomRegion.setY( (zoomRegion.height() - size.height()) / 2 );
		zoomRegion.setHeight( size.height() );
	}

	pixmap = QPixmap::fromImage( pixmap.toImage().copy( zoomRegion ) );
}

void RhdcHackWidget::loadThumbnail() {
	if( m_thumbnailLoaded ) return;

	const QString thumbnailPath = (BaseDir::cache() / _NFS("thumbnails") / fs::to_path(m_hack.info.hackId + ".png")).u8string().c_str();

	ThumbnailNotifier *notifier;
	auto i = s_thumbnailDownloaders.find( m_hack.info.hackId );
	if( i == s_thumbnailDownloaders.end() ) {
		QPixmap thumbnail( thumbnailPath, "PNG" );
		if( !thumbnail.isNull() ) {
			m_ui->thumbnail->setPixmap( thumbnail );
			m_thumbnailLoaded = true;
			return;
		}

		notifier = new ThumbnailNotifier();
		s_thumbnailDownloaders[m_hack.info.hackId] = notifier;

		const QString tempPath = (BaseDir::temp() / Uuid::Random().toString()).u8string().c_str();
		QFile *file = new QFile( tempPath );
		file->open( QIODevice::WriteOnly | QIODevice::Truncate );

		const string hackId = m_hack.info.hackId;
		RhdcApi::downloadThumbnail( hackId, file,
			[=](){
				file->close();

				QPixmap thumbnail( tempPath );
				if( thumbnail.isNull() ) {
					file->remove();
					file->deleteLater();
					return;
				}

				scaleAndZoom( thumbnail, QSize( 128, 96 ) );
				fs::create_directories( BaseDir::cache() / _NFS("thumbnails") );
				thumbnail.save( thumbnailPath, "PNG" );

				file->remove();
				file->deleteLater();

				notifier->notifyDownloaded();
				notifier->deleteLater();
				s_thumbnailDownloaders.erase( hackId );
			},
			[=](ApiErrorType) {
				file->close();
				file->remove();
				file->deleteLater();

				notifier->deleteLater();
				s_thumbnailDownloaders.erase( hackId );
			}
		);
	} else {
		notifier = i->second;
	}

	connect( notifier, &ThumbnailNotifier::downloaded, this, [=](){
		QPixmap thumbnail( thumbnailPath, "PNG" );
		if( thumbnail.isNull() ) return;

		this->m_ui->thumbnail->setPixmap( thumbnail );
		this->m_thumbnailLoaded = true;
	});

}

void RhdcHackWidget::paintEvent( QPaintEvent *event ) {
	loadThumbnail();
	QWidget::paintEvent( event );
}

void RhdcHackWidget::editorResized( int width ) {
	setFixedWidth( width );
}

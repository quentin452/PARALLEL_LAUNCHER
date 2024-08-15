#include "src/ui/now-playing-widget.hpp"
#include "ui_now-playing-widget.h"

#include <cstdio>
#include <QLabel>
#include "src/core/numeric-string.hpp"
#include "src/core/time.hpp"
#include "src/ui/icons.hpp"
#include "src/db/data-provider.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/rhdc/core/hack.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/retroarch.hpp"
#include "src/polyfill/base-directory.hpp"

static inline QLabel *makeLayoutWarning( QWidget *parent ) {
	QLabel *warning = new QLabel( NowPlayingWidget::tr( "The hack author has not provided a star layout for this hack. This layout will show unattainable stars and may be missing some others." ), parent );
	warning->setStyleSheet( "background-color: yellow;\nborder-radius: 8px;\npadding: 4px;\ncolor: #232627;" );
	warning->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
	warning->setTextFormat( Qt::PlainText );
	warning->setWordWrap( true );
	return warning;
}

NowPlayingWidget::NowPlayingWidget(
	QWidget *parent,
	AsyncProcess *process,
	const fs::path &romPath,
	const string &sha1,
	int64 pastPlayTime,
	const StarLayout *layoutOverride,
	bool *shouldReloadPtr
) :
	QWidget( parent ),
	m_ui( new Ui::NowPlayingWidget ),
	m_process( process ),
	m_starDisplay( nullptr ),
	m_timer( this ),
	m_pastPlayTime( pastPlayTime ),
	m_startTime( Time::nowMs() )
{
	m_ui->setupUi( this );

	RhdcHack hack;
	bool isRhdcHack = DataProvider::tryFetchRhdcHackByChecksum( sha1, &hack );
	m_ui->romNameLabel->setText( isRhdcHack ? hack.info.name.c_str() : romPath.stem().u8string().c_str() );

	if( isRhdcHack || layoutOverride != nullptr ) {
		const RhdcSettings &rhdcSettings = FileController::loadRhdcSettings();
		if( rhdcSettings.enableStarDisplay ) {
			StarLayout layout;
			if( layoutOverride || StarLayout::tryLoadLayout( hack.info.hackId, layout ) ) {
				if( layoutOverride ) layout = *layoutOverride;
				m_starDisplay = new LiveStarDisplayWidget( this, RetroArch::getSaveFilePath( romPath ), std::move( layout ) );
				QBoxLayout* centralLayout = qobject_cast<QBoxLayout*>( this->layout() );
				int centralLayoutCount = centralLayout->count();
				centralLayout->insertWidget( centralLayoutCount - 1, m_starDisplay, 1 );
			} else if( rhdcSettings.enableDefaultLayouts ) {
				layout = StarLayout::createDefault();
				m_starDisplay = new LiveStarDisplayWidget( this, RetroArch::getSaveFilePath( romPath ), std::move( layout ) );
				QBoxLayout* centralLayout = qobject_cast<QBoxLayout*>( this->layout() );
				int centralLayoutCount = centralLayout->count();
				centralLayout->insertWidget( centralLayoutCount - 1, makeLayoutWarning( this ) );
				centralLayout->insertWidget( centralLayoutCount, m_starDisplay, 1 );
			}
		}
	}

	connect( &m_timer, SIGNAL(timeout()), this, SLOT(updateTimers()) );
	m_timer.start( 1000 );

	setWindowIcon( Icon::appIcon() );
	m_ui->killButton->setIcon( Icon::pkill() );

	m_ui->reloadButton->setIcon( Icon::refresh() );
	connect( m_ui->reloadButton, &QPushButton::clicked, [=](){
		*shouldReloadPtr = true;
		this->killEmulator();
	});

	updateTimers();
}

NowPlayingWidget::~NowPlayingWidget() {
	delete m_ui;
}

int64 NowPlayingWidget::getSessionTime() const {
	return Time::nowMs() - m_startTime;
}

static string formatTime( int64 ms ) {
	static constexpr int64 MINUTE = 60;
	static constexpr int64 HOUR = MINUTE * 60;

	const int64 totalSeconds = (ms + 500ll) / 1000ll;

	const int64 seconds = totalSeconds % MINUTE;
	const int64 minutes = (totalSeconds % HOUR) / MINUTE;
	const int64 hours = totalSeconds / HOUR;

	if( hours > 0 ) {
		char mss[9];
		std::sprintf( mss, ":%02lld:%02lld", minutes, seconds );
		return Number::toString( hours ) + mss;
	} else {
		char mss[8];
		std::sprintf( mss, "%lld:%02lld", minutes, seconds );
		return string( mss );
	}
}

void NowPlayingWidget::killEmulator() {
	m_process->kill();
}

void NowPlayingWidget::updateTimers() {
	const int64 sessionTime = getSessionTime();
	m_ui->sessionTimeLabel->setText( formatTime( sessionTime ).c_str() );
	m_ui->totalTimeLabel->setText( formatTime( sessionTime + m_pastPlayTime ).c_str() );
}

#include "src/ui/date-time-offset-edit.hpp"

#include <QLineEdit>
#include <climits>

DateTimeOffsetEdit::DateTimeOffsetEdit( QWidget *parent ) :
	QDateTimeEdit( parent ),
	m_timer( this ),
	m_offset( 0 ),
	m_ignoreUpdate( false )
{
	connect( &m_timer, &QTimer::timeout, this, &DateTimeOffsetEdit::syncDate );
	connect( this, &QDateTimeEdit::dateTimeChanged, this, &DateTimeOffsetEdit::syncOffset );
	connect( lineEdit(), &QLineEdit::editingFinished, this, &DateTimeOffsetEdit::doneEditing );
}

void DateTimeOffsetEdit::setOffset( int seconds ) {
	m_offset = seconds;
	m_ignoreUpdate = true;
	setDateTime( QDateTime::currentDateTime().addSecs( m_offset ) );
	m_ignoreUpdate = false;
}

void DateTimeOffsetEdit::hideEvent( QHideEvent *event ) {
	m_timer.stop();
	QDateTimeEdit::hideEvent( event );
}

void DateTimeOffsetEdit::showEvent( QShowEvent *event ) {
	m_timer.start( 1000 - QDateTime::currentDateTime().time().msec() );
	QDateTimeEdit::showEvent( event );
}

void DateTimeOffsetEdit::syncOffset( const QDateTime &targetTime ) {
	if( m_ignoreUpdate ) return;
	const qint64 diff = QDateTime::currentDateTime().secsTo( targetTime );
	if( diff < (qint64)INT_MIN ) {
		m_offset = INT_MIN;
		m_ignoreUpdate = true;
		setDateTime( QDateTime::currentDateTime().addSecs( m_offset ) );
		m_ignoreUpdate = false;
	} else if( diff > (qint64)INT_MAX - 61 ) {
		m_offset = INT_MAX - 61;
		m_ignoreUpdate = true;
		setDateTime( QDateTime::currentDateTime().addSecs( m_offset ) );
		m_ignoreUpdate = false;
	} else {
		m_offset = (int)diff;
	}
}

void DateTimeOffsetEdit::syncDate() {
	if( !hasFocus() || !isEnabled() ) {
		QDateTime dateTime = QDateTime::currentDateTime().addSecs( m_offset );
		const qint64 ms = dateTime.time().msec();
		dateTime = dateTime.addMSecs( (qint64)((ms > 500) ? 1000 : 0) - ms );

		m_ignoreUpdate = true;
		setDateTime( dateTime );
		m_ignoreUpdate = false;
	}

	if( m_timer.interval() != 1000 ) {
		m_timer.start( 1000 );
	}
}

void DateTimeOffsetEdit::doneEditing() {
	syncOffset( dateTime() );
	clearFocus();
}

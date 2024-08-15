#include "src/ui/log-viewer-dialog.hpp"
#include "ui_log-viewer-dialog.h"

#include <QVBoxLayout>
#include <QDateTime>
#include "src/db/data-provider.hpp"
#include "src/core/time.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

static const int64 s_programStartTime = Time::nowMs();

LogViewerEntry::LogViewerEntry( QWidget *parent, LogEntry &&entry ) :
	QFrame( parent ),
	m_entry( entry ),
	m_timestampLabel( new QLabel( this ) ),
	m_messageLabel( new QLabel( this ) )
{
	setFrameStyle( QFrame::NoFrame | QFrame::Plain );
	setAutoFillBackground( true );
	setLayout( new QVBoxLayout( this ) );

	switch( m_entry.level ) {
		case LogLevel::Debug:
			setStyleSheet( "background-color: #14931f;" );
			break;
		case LogLevel::Info:
			setStyleSheet( "background-color: #0a56ab;" );
			break;
		case LogLevel::Warn:
			setStyleSheet( "background-color: #bb9800;" );
			break;
		case LogLevel::Error:
			setStyleSheet( "background-color: #c80000;" );
			break;
		case LogLevel::Fatal:
			setStyleSheet( "background-color: #000000;" );
			break;
	}

	m_timestampLabel->setTextFormat( Qt::PlainText );
	m_messageLabel->setTextFormat( Qt::PlainText );

	m_timestampLabel->setText( QDateTime::fromMSecsSinceEpoch( m_entry.timestamp ).toLocalTime().toString( Qt::RFC2822Date ) );
	m_messageLabel->setText( m_entry.message.c_str() );

	m_timestampLabel->setStyleSheet( "color: white; font-weight: bold;" );
	m_messageLabel->setStyleSheet( "color: white;" );

	m_messageLabel->setWordWrap( true );

	layout()->addWidget( m_timestampLabel );
	layout()->addWidget( m_messageLabel );
}

LogViewerEntry::~LogViewerEntry() {
	layout()->removeWidget( m_timestampLabel );
	layout()->removeWidget( m_messageLabel );
	delete m_timestampLabel;
	delete m_messageLabel;
	layout()->deleteLater();
}

LogViewerDialog::LogViewerDialog( QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::LogViewerDialog )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
	UiUtil::fixDialogButtonsOnWindows( this );

	std::vector<LogEntry> logs = DataProvider::fetchLogs();
	for( LogEntry &log : logs ) {
		LogViewerEntry *entry = new LogViewerEntry( m_ui->logContainer->widget(), std::move( log ) );
		m_ui->logContainer->widget()->layout()->addWidget( entry );
	}

	static_cast<QVBoxLayout*>( m_ui->logContainer->widget()->layout() )->addStretch( 1 );
}

LogViewerDialog::~LogViewerDialog() {
	while( true ) {
		QLayoutItem *item = m_ui->logContainer->widget()->layout()->takeAt( 0 );
		if( item == nullptr || item->widget() == nullptr ) break;
		if( item->layout() ) item->layout()->deleteLater();
		if( item->widget() ) item->widget()->deleteLater();
		delete item;
	}
}

void LogViewerDialog::updateLogVisibility() {
	for( int i = 0; i < m_ui->logContainer->widget()->layout()->count(); i++ ) {
		LogViewerEntry *entry = dynamic_cast<LogViewerEntry*>( m_ui->logContainer->widget()->layout()->itemAt( i )->widget() );
		if( entry == nullptr ) continue;

		entry->setTimestampVisible( m_ui->timestampCheckbox->isChecked() );
		if( m_ui->sessionCheckbox->isChecked() && entry->entry().timestamp < s_programStartTime ) {
			entry->setVisible( false );
			continue;
		}

		switch( entry->entry().level ) {
			case LogLevel::Debug:
				entry->setVisible( m_ui->debugCheckbox->isChecked() );
				break;
			case LogLevel::Info:
				entry->setVisible( m_ui->infoCheckbox->isChecked() );
				break;
			case LogLevel::Warn:
				entry->setVisible( m_ui->warnCheckbox->isChecked() );
				break;
			case LogLevel::Error:
				entry->setVisible( m_ui->errorCheckbox->isChecked() );
				break;
			case LogLevel::Fatal:
				entry->setVisible( m_ui->fatalCheckbox->isChecked() );
				break;
		}
	}
}

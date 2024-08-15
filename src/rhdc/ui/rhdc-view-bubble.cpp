#include "src/rhdc/ui/rhdc-view-bubble.hpp"
#include "ui_rhdc-view-bubble.h"

#include "src/core/file-controller.hpp"

RhdcViewBubble::RhdcViewBubble( QWidget *parent, QPushButton *target ) :
	QWidget( parent ),
	m_ui( new Ui::RhdcViewBubble ),
	m_target( target )
{
	m_ui->setupUi( this );

	connect( target, &QPushButton::clicked, this, &RhdcViewBubble::dismiss );
	connect( &m_timer, &QTimer::timeout, this, &RhdcViewBubble::reposition );
	reposition();

	m_timer.setInterval( 16 );
	m_timer.start();
	hide();
}

RhdcViewBubble::~RhdcViewBubble() {
	m_timer.stop();
	delete m_ui;
}

void RhdcViewBubble::dismiss() {
	if( parent() == nullptr ) return;

	AppSettings settings = FileController::loadAppSettings();
	settings.knowsAboutRhdcView = true;
	FileController::saveAppSettings( settings );

	hide();
	setParent( nullptr );
	deleteLater();
	m_timer.stop();
}

void RhdcViewBubble::reposition() {
	if( parent() == nullptr ) return;

	if( !m_target->isVisible() ) {
		hide();
		return;
	}

	show();
	const QPoint position = parentWidget()->mapFromGlobal(
		m_target->mapToGlobal(
			QPoint( m_target->width() / 2, m_target->height() )
		)
	);

	adjustSize();
	setGeometry(
		position.x() - 264,
		position.y(),
		320,
		height()
	);
}

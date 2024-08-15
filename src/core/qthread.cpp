#include "src/core/qthread.hpp"

#include <QMetaObject>
#include <QCoreApplication>
#include <thread>
#include <exception>

void QtThread::safeAsync( const std::function<void(void)> &task ) {
	QMetaObject::invokeMethod( QCoreApplication::instance(), task, Qt::QueuedConnection );
}

void QtThread::execOnUiThread( const std::function<void(void)> &task ) {
	QMetaObject::invokeMethod( QCoreApplication::instance(), task, Qt::AutoConnection );
}

template<> void QtThread::safeWait<void>( const std::function<void(void)> &task ) {
	bool finished = false;
	safeAsync( [&task,&finished](){
		task();
		finished = true;
	});

	QCoreApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
	while( !finished ) {
		std::this_thread::yield();
		QCoreApplication::processEvents( QEventLoop::ExcludeUserInputEvents );
	}
}

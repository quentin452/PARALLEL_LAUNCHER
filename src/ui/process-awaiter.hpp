#ifndef SRC_UI_PROCESS_AWAITER_HPP_
#define SRC_UI_PROCESS_AWAITER_HPP_

#include <QThread>
#include "src/polyfill/process.hpp"

class _ProcessAwaiterThread : public QThread {
	Q_OBJECT

	private:
	AsyncProcess *const m_process;

	public:
	inline _ProcessAwaiterThread( AsyncProcess *process ) : QThread( nullptr ), m_process( process ) {}
	inline virtual ~_ProcessAwaiterThread() {}

	protected:
	inline void run() override {
		const int64 exitCode = m_process->join();
		emit processExited( exitCode );
	}

	signals:
	void processExited( int64 exitCode );
};

namespace ProcessAwaiter {
	extern void QtSafeAwait(
		AsyncProcess *process,
		const std::function<void( int64 exitCode, int64 runtimeMs )> &callback
	);
}



#endif /* SRC_UI_PROCESS_AWAITER_HPP_ */

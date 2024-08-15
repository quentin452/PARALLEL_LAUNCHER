#include "src/core/thread-safety.hpp"

#include <thread>
#include "src/polyfill/crash.hpp"

static std::thread::id s_mainThreadId;

void ThreadSafety::registerMainThread() noexcept {
	s_mainThreadId = std::this_thread::get_id();
}

void ThreadSafety::assertThreadIsSafe() noexcept {
	const std::thread::id currentThreadId = std::this_thread::get_id();
#ifdef DEBUG
	pl_assert( currentThreadId == s_mainThreadId );
#else
	if( currentThreadId != s_mainThreadId ) {
		logWarningReport( "Unsafe async call", nullptr );
	}
#endif
}

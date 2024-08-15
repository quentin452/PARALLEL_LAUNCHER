#ifndef SRC_CORE_THREAD_SAFETY_HPP_
#define SRC_CORE_THREAD_SAFETY_HPP_

namespace ThreadSafety {

	extern void registerMainThread() noexcept;
	extern void assertThreadIsSafe() noexcept;
	
}

#endif /* SRC_CORE_THREAD_SAFETY_HPP_ */

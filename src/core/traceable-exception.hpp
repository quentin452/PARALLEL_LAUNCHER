#ifndef SRC_CORE_TRACEABLE_EXCEPTION_HPP_
#define SRC_CORE_TRACEABLE_EXCEPTION_HPP_

#include "src/polyfill/backtrace.hpp"

class TraceableException {

	private:
	const Backtrace m_backtrace;

	protected:
	TraceableException() noexcept : m_backtrace() {};

	public:
	virtual ~TraceableException() {};

	inline const Backtrace &backtrace() const noexcept { return m_backtrace; }

};

#endif /* SRC_CORE_TRACEABLE_EXCEPTION_HPP_ */

#ifndef SRC_POLYFILL_BACKTRACE_HPP_
#define SRC_POLYFILL_BACKTRACE_HPP_

#ifdef _WIN32
#include <cstdio>
#endif

class Backtrace {

	private:
	void *m_trace[32];
	int m_size;

	public:
	Backtrace() noexcept;

#ifdef _WIN32
	void log( std::FILE *logFile ) const noexcept;
	static void logFromContext( std::FILE *logFile, void *context ) noexcept;
#else
	void log( int fd ) const noexcept;
#endif

};

#endif /* SRC_POLYFILL_BACKTRACE_HPP_ */

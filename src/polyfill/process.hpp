#ifndef SRC_POLYFILL_PROCESS_HPP_
#define SRC_POLYFILL_PROCESS_HPP_

#include "src/core/traceable-exception.hpp"
#include "src/types.hpp"
#include <stdexcept>
#include <memory>
#include <vector>

class ProcessLaunchException : public std::runtime_error, TraceableException {

	private:
	const string m_process;
	const std::vector<string> m_args;
	const int m_errorCode;

	public:
	ProcessLaunchException(
		const string &process,
		const std::vector<string> &args,
		int errorCode
	) :
		std::runtime_error( "Failed to launch child process" ),
		TraceableException(),
		m_process( process ),
		m_args( args ),
		m_errorCode( errorCode )
	{}

	inline const string &process() const noexcept { return m_process; }
	inline const std::vector<string> &processArgs() const noexcept { return m_args; }
	inline int errorCode() const noexcept { return m_errorCode; }

};

class AsyncProcess {

	private:
	std::shared_ptr<void> m_implData;

	public:
	AsyncProcess() noexcept : m_implData( nullptr ) {}

	AsyncProcess( const string &process, const std::vector<string> &args );
#ifdef _WIN32
	// Necessary because of horrible Windows scuff
	AsyncProcess( const string &rawCmd ); // does not do any escaping

	static string quoteAndEscapeArg( const string &arg );
#endif

	AsyncProcess( const AsyncProcess &other ) = delete;
	AsyncProcess( AsyncProcess &&other ) noexcept;

	~AsyncProcess() {};

	AsyncProcess &operator=( const AsyncProcess &other ) = delete;
	AsyncProcess &operator=( AsyncProcess &&other ) noexcept;

	int64 pid() const noexcept;
	bool running() const noexcept;

	int64 join();
	void detach();
	bool kill();

};

namespace Process {

	extern bool tryGetOutput(
		const string &command,
		string &output
	);

	extern bool tryGetErrorOutput(
		const string &command,
		string &output
	);

}



#endif /* SRC_POLYFILL_PROCESS_HPP_ */

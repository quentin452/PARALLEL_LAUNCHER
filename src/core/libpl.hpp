#ifndef SRC_CORE_LIBPL_HPP_
#define SRC_CORE_LIBPL_HPP_

#include <thread>
#include "src/core/rom.hpp"
#include "src/polyfill/fifo-pipe.hpp"

class LibplHandler {

	private:
	ReadableFifoPipe m_input;
	WritableFifoPipe m_output;
	std::thread m_thread;

	public:
	inline LibplHandler() noexcept {};
	inline ~LibplHandler() { disconnect(); }

	LibplHandler( const LibplHandler &other ) = delete;
	LibplHandler( LibplHandler &&other ) = delete;

	int initialize();

	void connect( const RomInfo &romInfo );
	void disconnect();

	inline const std::string &inputPipePath() const noexcept {
		return m_input.filePath();
	}

	inline const std::string &outputPipePath() const noexcept {
		return m_output.filePath();
	}

};

#endif

#ifndef SRC_POLYFILL_FILE_HPP_
#define SRC_POLYFILL_FILE_HPP_

#include <fstream>
#include "src/core/filesystem.hpp"

class InputFile : public std::ifstream {
#ifdef _WIN32
	private:
	char *m_buffer;
#endif

	public:
	InputFile( const fs::path &filePath, bool binary );
	virtual ~InputFile();

	inline InputFile( InputFile &&other ) : std::ifstream( std::move( other ) ) {
#ifdef _WIN32
		m_buffer = other.m_buffer;
		other.m_buffer = nullptr;
#endif
	}
};

class OutputFile : public std::ofstream {
#ifdef _WIN32
	private:
	char *m_buffer;
#endif

	public:
	OutputFile( const fs::path &filePath, bool binary );
	virtual ~OutputFile();

	inline OutputFile( OutputFile &&other ) : std::ofstream( std::move( other ) ) {
#ifdef _WIN32
		m_buffer = other.m_buffer;
		other.m_buffer = nullptr;
#endif
	}
};

class File : public std::fstream {
#ifdef _WIN32
	private:
	char *m_buffer;
#endif

	public:
	File( const fs::path &filePath, bool binary );
	virtual ~File();

	inline File( File &&other ) : std::fstream( std::move( other ) ) {
#ifdef _WIN32
		m_buffer = other.m_buffer;
		other.m_buffer = nullptr;
#endif
	}
};

#endif /* SRC_POLYFILL_FILE_HPP_ */

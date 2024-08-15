#include "src/polyfill/file.hpp"

#include <ios>

#if defined(_WIN32)
	static constexpr size_t BUFFER_SIZE = 8192;

	InputFile::InputFile( const fs::path &filePath, bool binary ) :
		std::ifstream(),
		m_buffer( new char[BUFFER_SIZE] )
	{
		rdbuf()->pubsetbuf( m_buffer, BUFFER_SIZE );
		open( filePath, binary ? (std::ios_base::in | std::ios_base::binary) : std::ios_base::in );
	}

	InputFile::~InputFile() {
		if( m_buffer != nullptr ) {
			rdbuf()->pubsetbuf( nullptr, 0 );
			delete[] m_buffer;
			m_buffer = nullptr;
		}
	}

	OutputFile::OutputFile( const fs::path &filePath, bool binary ) :
		std::ofstream(),
		m_buffer( new char[BUFFER_SIZE] )
	{
		rdbuf()->pubsetbuf( m_buffer, BUFFER_SIZE );
		open( filePath, binary ? (std::ios_base::out | std::ios_base::trunc | std::ios_base::binary) : (std::ios_base::in | std::ios_base::trunc) );
	}

	OutputFile::~OutputFile() {
		if( m_buffer != nullptr ) {
			flush();
			rdbuf()->pubsetbuf( nullptr, 0 );
			delete[] m_buffer;
			m_buffer = nullptr;
		}
	}

	File::File( const fs::path &filePath, bool binary ) :
		std::fstream(),
		m_buffer( new char[BUFFER_SIZE] )
	{
		rdbuf()->pubsetbuf( m_buffer, BUFFER_SIZE );
		open( filePath, binary ? (std::ios_base::in | std::ios_base::out | std::ios_base::binary) : (std::ios_base::in | std::ios_base::out) );
	}

	File::~File() {
		if( m_buffer != nullptr ) {
			flush();
			rdbuf()->pubsetbuf( nullptr, 0 );
			delete[] m_buffer;
			m_buffer = nullptr;
		}
	}
#elif defined(__APPLE__)
	InputFile::InputFile( const fs::path &filePath, bool binary ) : std::ifstream(
		filePath.u8string(),
		binary ? (std::ios_base::in | std::ios_base::binary) : std::ios_base::in
	) {}

	InputFile::~InputFile() {}

	OutputFile::OutputFile( const fs::path &filePath, bool binary ) : std::ofstream(
		filePath.u8string(),
		binary ? (std::ios_base::out | std::ios_base::trunc | std::ios_base::binary) : (std::ios_base::in | std::ios_base::trunc)
	) {}

	OutputFile::~OutputFile() {}

	File::File( const fs::path &filePath, bool binary ) : std::fstream(
		filePath.u8string(),
		binary ? (std::ios_base::in | std::ios_base::out | std::ios_base::binary) : (std::ios_base::in | std::ios_base::out)
	) {}

	File::~File() {}
#else
	InputFile::InputFile( const fs::path &filePath, bool binary ) : std::ifstream(
		filePath,
		binary ? (std::ios_base::in | std::ios_base::binary) : std::ios_base::in
	) {}

	InputFile::~InputFile() {}

	OutputFile::OutputFile( const fs::path &filePath, bool binary ) : std::ofstream(
		filePath,
		binary ? (std::ios_base::out | std::ios_base::trunc | std::ios_base::binary) : (std::ios_base::in | std::ios_base::trunc)
	) {}

	OutputFile::~OutputFile() {}

	File::File( const fs::path &filePath, bool binary ) : std::fstream(
		filePath,
		binary ? (std::ios_base::in | std::ios_base::out | std::ios_base::binary) : (std::ios_base::in | std::ios_base::out)
	) {}

	File::~File() {}
#endif

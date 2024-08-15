#ifndef SRC_CORE_BUFFER_HPP_
#define SRC_CORE_BUFFER_HPP_

#include <cstdlib>

class Buffer {

	private:
	char *m_data;

	public:
	inline Buffer() : m_data( nullptr ) {}
	inline Buffer( size_t size ) : m_data( new char[size] ) {}

	Buffer( const Buffer &other ) = delete;
	inline Buffer( Buffer &&other ) : m_data( other.m_data ) {
		other.m_data = nullptr;
	}

	~Buffer() {
		if( m_data != nullptr ) delete[] m_data;
	}

	inline Buffer &operator=( Buffer &&other ) {
		if( m_data != nullptr ) delete[] m_data;
		m_data = other.m_data;
		other.m_data = nullptr;
		return *this;
	}

	inline char *data() noexcept { return m_data; }
	inline const char *data() const noexcept { return m_data; }

	inline unsigned char *udata() noexcept { return (unsigned char*)m_data; }
	inline const unsigned char *udata() const noexcept { return (const unsigned char*)m_data; }

};

class CBuffer {

	private:
	void *m_ptr;

	public:
	inline CBuffer( void *ptr ) : m_ptr( ptr ) {}
	~CBuffer() {
		if( m_ptr != nullptr ) {
			std::free( m_ptr );
			m_ptr = nullptr;
		}
	}

	inline bool isNull() noexcept { return m_ptr == nullptr; }

	template<typename T> T *data() noexcept { return (T*)m_ptr; }
	template<typename T> const T *data() const noexcept { return (const T*)m_ptr; }

};

#endif /* SRC_CORE_BUFFER_HPP_ */

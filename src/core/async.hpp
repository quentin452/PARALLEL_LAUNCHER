#ifndef SRC_CORE_ASYNC_HPP_
#define SRC_CORE_ASYNC_HPP_

#include <memory>
#include <atomic>
#include <condition_variable>

class CancellationToken {

	private:
	std::shared_ptr<volatile bool> m_value;

	public:
	CancellationToken() {
		m_value = std::shared_ptr<volatile bool>( new volatile bool( false ) );
	}

	CancellationToken( const CancellationToken &other ) noexcept {
		m_value = other.m_value;
	}

	CancellationToken( CancellationToken &&other ) noexcept {
		m_value = std::move( other.m_value );
	}

	inline CancellationToken &operator=( const CancellationToken &other ) noexcept {
		m_value = other.m_value;
		return *this;
	}

	inline CancellationToken &operator=( CancellationToken &&other ) noexcept {
		m_value = std::move( other.m_value );
		return *this;
	}


	inline void cancel() noexcept {
		*m_value = true;
	}

	inline bool isCancelled() const noexcept {
		return *m_value;
	}

};

#endif /* SRC_CORE_ASYNC_HPP_ */

#ifndef SRC_CORE_RATE_LIMITER_HPP_
#define SRC_CORE_RATE_LIMITER_HPP_

#include <vector>
#include "src/types.hpp"

class RateLimiter {

	private:
	const int64 m_interval;
	size_t m_start;
	size_t m_length;
	std::vector<int64> m_ringBuffer;

	public:
	inline RateLimiter( size_t limit, int64 interval ) : m_interval( interval ), m_start( 0 ), m_length( 0 ), m_ringBuffer( limit, 0ll ) {}
	inline ~RateLimiter() {}

	bool check();

	inline void pop() noexcept { m_length--; }
	inline void clear() noexcept { m_length = 0; }

};

#endif

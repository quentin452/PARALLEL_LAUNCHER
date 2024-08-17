#include "src/core/rate-limiter.hpp"

#include "src/core/time.hpp"

bool RateLimiter::check() {
  const int64 now = Time::nowMs();

  for (size_t i = m_start; m_length != 0; ++i %= m_ringBuffer.size()) {
    if (now - m_ringBuffer[i] < m_interval)
      break;
    ++m_start %= m_ringBuffer.size();
    m_length--;
  }

  if (m_length == m_ringBuffer.size()) {
    return false;
  }

  m_ringBuffer[m_start + m_length % m_ringBuffer.size()] = now;
  m_length++;
  return true;
}

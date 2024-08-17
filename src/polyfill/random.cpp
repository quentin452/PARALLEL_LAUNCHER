#include "src/polyfill/random.hpp"

#ifdef _WIN32
#include <Windows.h>

#include <bcrypt.h>
#else
#include <fstream>
#endif

#include "src/types.hpp"
#include <atomic>
#include <chrono>
#include <cstring>
#include <random>
#include <thread>

static inline std::uint_fast64_t getInitialSeed() {
  return std::chrono::system_clock::now().time_since_epoch().count();
}

static void fallbackGetBytes(void *buffer, size_t count) {
  static std::mt19937_64 s_random(getInitialSeed());
  static std::atomic<bool> s_locked(false);

  while (s_locked.exchange(true, std::memory_order_relaxed))
    std::this_thread::yield();
  std::atomic_thread_fence(std::memory_order_acquire);

  uint64 *tempBuffer = nullptr;
  try {
    if (count % sizeof(uint64) == 0 &&
        (uintptr_t)buffer % alignof(uint64) == 0) {
      uint64 *ibuffer = reinterpret_cast<uint64 *>(buffer);
      for (size_t i = 0; i < count / sizeof(uint64); i++) {
        ibuffer[i] = s_random();
      }
    } else {
      tempBuffer = new uint64[(count / sizeof(uint64)) + 1];
      for (size_t i = 0; i < count / sizeof(uint64); i++) {
        tempBuffer[i] = s_random();
      }

      std::memcpy(buffer, tempBuffer, count);
      delete[] tempBuffer;
    }
  } catch (...) {
    std::atomic_thread_fence(std::memory_order_release);
    s_locked.store(false, std::memory_order_relaxed);
    if (tempBuffer)
      delete[] tempBuffer;
    throw;
  }

  std::atomic_thread_fence(std::memory_order_release);
  s_locked.store(false, std::memory_order_relaxed);
}

#ifdef _WIN32
void RNG::getBytes(void *buffer, size_t count) {
  const NTSTATUS status =
      BCryptGenRandom(nullptr, reinterpret_cast<unsigned char *>(buffer),
                      (ULONG)count, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
  if (status)
    fallbackGetBytes(buffer, count);
}
#else
void RNG::getBytes(void *buffer, size_t count) {
  thread_local std::ifstream randomStream("/dev/urandom");
  randomStream.read(reinterpret_cast<char *>(buffer), count);
  if (randomStream.fail())
    fallbackGetBytes(buffer, count);
}
#endif

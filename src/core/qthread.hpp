#ifndef SRC_CORE_QTHREAD_HPP_
#define SRC_CORE_QTHREAD_HPP_

#include <functional>

namespace QtThread {

extern void safeAsync(const std::function<void(void)> &task);
extern void execOnUiThread(const std::function<void(void)> &task);

template <typename T> T safeWait(const std::function<T(void)> &task);
template <> void safeWait<void>(const std::function<void(void)> &task);
template <typename T> T safeWait(const std::function<T(void)> &task) {
  T returnValue;
  safeWait<void>([&returnValue, &task]() { returnValue = task(); });
  return returnValue;
}

} // namespace QtThread

#endif /* SRC_CORE_QTHREAD_HPP_ */

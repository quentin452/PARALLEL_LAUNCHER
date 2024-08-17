#include "src/ui/process-awaiter.hpp"

#include <chrono>

void ProcessAwaiter::QtSafeAwait(
    AsyncProcess *process,
    const std::function<void(int64 exitCode, int64 runtimeMs)> &callback) {
  const auto startTime = std::chrono::steady_clock::now();
  _ProcessAwaiterThread *qthread = new _ProcessAwaiterThread(process);
  QObject::connect(
      qthread, &_ProcessAwaiterThread::processExited, qthread,
      [=](int64 exitCode) {
        const int64 runtime =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime)
                .count();
        callback(exitCode, runtime);
        qthread->deleteLater();
      },
      Qt::QueuedConnection);
  qthread->start();
}

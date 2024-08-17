#ifndef SRC_POLYFILL_CRASH_HPP_
#define SRC_POLYFILL_CRASH_HPP_

#include "src/polyfill/backtrace.hpp"
#include <cstdlib>

extern void __log_error_internal(const char *type, const char *message,
                                 bool isWarning, const Backtrace &backtrace);

extern void registerCrashHandlers();
extern void storeCrashLog();
extern void logWarningReport(const char *warningType, const char *message);

#define PL_STRINGIFY_2(x) #x
#define PL_STRINGIFY(x) PL_STRINGIFY_2(x)

#define pl_assert(requirement)                                                 \
  if (!(requirement)) {                                                        \
    __log_error_internal("Assertion Failed",                                   \
                         "" #requirement "\n" __FILE__                         \
                         ":" PL_STRINGIFY(__LINE__),                           \
                         false, Backtrace());                                  \
    std::abort();                                                              \
  }

#define pl_crash(errorType, message)                                           \
  __log_error_internal(errorType, message, false, Backtrace());                \
  std::abort();

#define logWarningReport(warningType, message)                                 \
  __log_error_internal(warningType, message, true, Backtrace())

#endif /* SRC_POLYFILL_CRASH_HPP_ */

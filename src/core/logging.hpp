#ifndef SRC_CORE_LOGGING_HPP_
#define SRC_CORE_LOGGING_HPP_

#include "src/types.hpp"
#include <vector>

enum class LogLevel : int {
  Debug = 0,
  Info = 1,
  Warn = 2,
  Error = 3,
  Fatal = 4
};

struct LogEntry {
  LogLevel level;
  int64 timestamp;
  string message;
};

namespace Logs {
extern void write(LogLevel level, const string &message);
extern std::vector<LogEntry> fetch();
} // namespace Logs

#ifdef DEBUG
#define logDebug(msg) Logs::write(LogLevel::Debug, msg)
#else
#define logDebug(msg)
#endif

#define logInfo(msg) Logs::write(LogLevel::Info, msg)
#define logWarn(msg) Logs::write(LogLevel::Warn, msg)
#define logError(msg) Logs::write(LogLevel::Error, msg)
#define logFatal(msg) Logs::write(LogLevel::Fatal, msg)

#endif /* SRC_CORE_LOGGING_HPP_ */

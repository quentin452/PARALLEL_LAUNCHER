#include "src/core/logging.hpp"

#include "src/core/qthread.hpp"
#include "src/core/time.hpp"
#include "src/db/data-provider.hpp"

#ifdef DEBUG
#include <iostream>

void Logs::write( LogLevel level, const string &message ) {
	switch( level ) {
		case LogLevel::Debug:
			std::cout << "[DEBUG] " << message << std::endl << std::flush; break;
		case LogLevel::Info:
			std::cout << "[INFO] " << message << std::endl << std::flush; break;
		case LogLevel::Warn:
			std::cout << "[WARN] " << message << std::endl << std::flush; break;
		case LogLevel::Error:
			std::cerr << "[ERROR] " << message << std::endl << std::flush; break;
		case LogLevel::Fatal:
			std::cerr << "[FATAL] " << message << std::endl << std::flush; break;
	}

	QtThread::execOnUiThread( [level,message]() {
		DataProvider::addLogEntry({
			level,
			Time::nowMs(),
			message
		});
	});
}
#else
void Logs::write( LogLevel level, const string &message ) {
	QtThread::execOnUiThread( [level,message]() {
		DataProvider::addLogEntry({
			level,
			Time::nowMs(),
			message
		});
	});
}
#endif

std::vector<LogEntry> Logs::fetch() {
	return DataProvider::fetchLogs();
}

#include "src/core/time.hpp"

#include <QDateTime>

int64 Time::tryParseIso( const string &dateString, bool &success ) {
	QDateTime dateTime = QDateTime::fromString( dateString.c_str(), Qt::DateFormat::ISODate );
	success = dateTime.isValid();
	return success ? dateTime.toSecsSinceEpoch() : 0L;
}

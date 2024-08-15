#ifndef SRC_CORE_TIME_HPP_
#define SRC_CORE_TIME_HPP_

#include <chrono>
#include <type_traits>
#include "src/types.hpp"

namespace Time {

	inline int64 now() {
		return (int64)std::chrono::duration_cast<std::chrono::seconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	}

	inline int64 nowMs() {
		return (int64)std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
	}

	template<typename Clock, typename Units> int64 toUnixTime( const std::chrono::time_point<Clock,Units> &timePoint ) {
		if constexpr( std::is_same_v<Clock,std::chrono::system_clock> ) {
			return (int64)std::chrono::duration_cast<std::chrono::seconds>( timePoint.time_since_epoch() ).count();
		} else {
			return Time::now() + (int64)std::chrono::duration_cast<std::chrono::seconds>( timePoint - std::chrono::time_point<Clock,Units>::clock::now() ).count();
		}
	}

	template<typename Clock, typename Units> int64 toUnixTimeMs( const std::chrono::time_point<Clock,Units> &timePoint ) {
		if constexpr( std::is_same_v<Clock,std::chrono::system_clock> ) {
			return (int64)std::chrono::duration_cast<std::chrono::milliseconds>( timePoint.time_since_epoch() ).count();
		} else {
			return Time::nowMs() + (int64)std::chrono::duration_cast<std::chrono::milliseconds>( timePoint - std::chrono::time_point<Clock,Units>::clock::now() ).count();
		}
	}

	extern int64 tryParseIso( const string &dateString, bool &success );

}

#endif /* SRC_CORE_TIME_HPP_ */

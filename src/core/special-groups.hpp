#ifndef SRC_CORE_SPECIAL_GROUPS_HPP_
#define SRC_CORE_SPECIAL_GROUPS_HPP_

#include <QString>
#include "src/types.hpp"

namespace SpecialGroups {

	extern const string Favourites;
	extern const string Uncategorized;
	extern const string WantToPlay;
	extern const string InProgress;
	extern const string Completed;

}

namespace GroupName {
	extern QString localize( const string &groupName );
	extern string getRhdcName( const string &groupName );
}



#endif /* SRC_CORE_SPECIAL_GROUPS_HPP_ */

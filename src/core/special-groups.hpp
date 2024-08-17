#ifndef SRC_CORE_SPECIAL_GROUPS_HPP_
#define SRC_CORE_SPECIAL_GROUPS_HPP_

#include "src/types.hpp"
#include <QString>

namespace SpecialGroups {

extern const string Favourites;
extern const string Uncategorized;
extern const string WantToPlay;
extern const string InProgress;
extern const string Completed;

} // namespace SpecialGroups

namespace GroupName {
extern QString localize(const string &groupName);
extern string getRhdcName(const string &groupName);
} // namespace GroupName

#endif /* SRC_CORE_SPECIAL_GROUPS_HPP_ */

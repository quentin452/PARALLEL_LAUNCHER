#include "src/core/special-groups.hpp"

#include <QCoreApplication>

const string SpecialGroups::Favourites = "\x01"
                                         "10";
const string SpecialGroups::WantToPlay = "\x01"
                                         "20";
const string SpecialGroups::InProgress = "\x01"
                                         "30";
const string SpecialGroups::Completed = "\x01"
                                        "40";
const string SpecialGroups::Uncategorized = "\x01"
                                            "90";

QString GroupName::localize(const string &groupName) {
  if (groupName == SpecialGroups::Favourites) {
    return QCoreApplication::translate("GroupName", "Favourites");
  } else if (groupName == SpecialGroups::Uncategorized) {
    return QCoreApplication::translate("GroupName", "Uncategorized");
  } else if (groupName == SpecialGroups::WantToPlay) {
    return QCoreApplication::translate("GroupName", "Want To Play");
  } else if (groupName == SpecialGroups::InProgress) {
    return QCoreApplication::translate("GroupName", "In Progress");
  } else if (groupName == SpecialGroups::Completed) {
    return QCoreApplication::translate("GroupName", "Completed");
  }

  return QString(groupName.c_str());
}

string GroupName::getRhdcName(const string &groupName) {
  if (groupName == SpecialGroups::Favourites) {
    return "Favorites";
  } else if (groupName == SpecialGroups::WantToPlay) {
    return "Want To Play";
  } else if (groupName == SpecialGroups::InProgress) {
    return "In Progress";
  } else if (groupName == SpecialGroups::Completed) {
    return "Completed";
  }

  return localize(groupName).toStdString();
}

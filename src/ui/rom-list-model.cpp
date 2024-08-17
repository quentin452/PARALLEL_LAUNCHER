#include "src/ui/rom-list-model.hpp"

#include "src/core/numeric-string.hpp"
#include "src/core/special-groups.hpp"
#include "src/db/data-provider.hpp"
#include <QDateTime>
#include <QFont>
#include <algorithm>
#include <cstdio>
#include <map>

using namespace __RomListModelPrivate;
typedef _RomListModelIndex Index;
typedef _RomListModelGroup Group;

static inline QString getName(const ConstRomReference &ref) {
  if (!ref.info->name.empty()) {
    return QString::fromUtf8(ref.info->name.c_str());
  } else if (ref.rhdc != nullptr && !ref.rhdc->info.name.empty()) {
    return QString::fromUtf8(ref.rhdc->info.name.c_str());
  } else {
    return QString::fromUtf8(ref.file->path.stem().u8string().c_str());
  }
}

static bool compareName(const ConstRomReference &a,
                        const ConstRomReference &b) {
  return QString::compare(getName(a), getName(b), Qt::CaseInsensitive) < 0;
}

static bool compareInternalName(const ConstRomReference &a,
                                const ConstRomReference &b) {
  return a.info->internalName < b.info->internalName;
}

static bool comparePath(const ConstRomReference &a,
                        const ConstRomReference &b) {
  return a.file->path < b.file->path;
}

static bool compareLastPlayed(const ConstRomReference &a,
                              const ConstRomReference &b) {
  return a.info->lastPlayed < b.info->lastPlayed;
}

static bool comparePlayTime(const ConstRomReference &a,
                            const ConstRomReference &b) {
  return a.info->playTime < b.info->playTime;
}

static bool (*s_romComparers[])(const ConstRomReference &,
                                const ConstRomReference &) = {
    compareName, compareInternalName, comparePath, compareLastPlayed,
    comparePlayTime};

static Group buildGroup(const string &groupName, size_t groupIndex,
                        const std::vector<size_t> &groupRoms) {
  std::vector<_RomListModelRow> children;
  children.reserve(groupRoms.size());

  int i = 0;
  for (const size_t romIndex : groupRoms) {
    _RomListModelRow props;
    for (int j = 0; j < NUM_COLUMNS; j++) {
      props[j] = Index{/* group */ (int)groupIndex,
                       /* row */ i,
                       /* col */ j,
                       /* lookupIndex */ (int)romIndex};
    }
    children.push_back(std::move(props));
    i++;
  }

  return Group{/* name */ groupName,
               /* self */
               Index{/* group */ (int)groupIndex,
                     /* row */ -1,
                     /* col */ 0,
                     /* lookupIndex */ -1},
               /* children */ std::move(children)};
}

void RomListModel::fullUpdate() {
  beginResetModel();

  m_groups.clear();
  m_romFiles = DataProvider::fetchAllRomFiles();
  m_romInfo = DataProvider::fetchAllRomInfo();
  m_rhdcMap = DataProvider::fetchAllRhdcHackVersions();
  m_rhdcInfo = DataProvider::fetchFollowedRhdcHacks();

  const std::vector<string> groupNames = DataProvider::fetchAllGroups();
  HashMap<string, std::vector<size_t>> groupMap;
  HashMap<fs::path, size_t> indexMap;

  size_t i = 0;
  for (const RomFile &rom : m_romFiles) {
    indexMap[rom.path] = i++;
    const std::set<string> &romGroups = m_romInfo.at(rom.sha1).groups;
    if (romGroups.empty()) {
      groupMap[SpecialGroups::Uncategorized].push_back(i - 1);
    } else
      for (const string &group : romGroups) {
        groupMap[group].push_back(i - 1);
      }
  }

  m_groups.reserve(groupNames.size() + 1);
  for (const string &groupName : groupNames) {
    m_groups.push_back(
        buildGroup(groupName, m_groups.size(), groupMap[groupName]));
  }
  if (groupMap.count(SpecialGroups::Uncategorized) > 0) {
    m_groups.push_back(buildGroup(SpecialGroups::Uncategorized, m_groups.size(),
                                  groupMap[SpecialGroups::Uncategorized]));
  }

  sortInternal();
  endResetModel();
}

void RomListModel::rhdcUpdate() {
  beginResetModel();
  m_rhdcMap = DataProvider::fetchAllRhdcHackVersions();
  m_rhdcInfo = DataProvider::fetchFollowedRhdcHacks();
  if (m_sortBy == COL_NAME)
    sortInternal();
  endResetModel();
}

void RomListModel::partialUpdate(const RomInfo &changedRom, bool nameChanged,
                                 bool playTimeChanged) {
  if ((playTimeChanged &&
       (m_sortBy == COL_LAST_PLAYED || m_sortBy == COL_PLAY_TIME)) ||
      (nameChanged && m_sortBy == COL_NAME)) {
    beginResetModel();
    m_romInfo[changedRom.sha1] = changedRom;
    sortInternal();
    endResetModel();
  } else {
    m_romInfo[changedRom.sha1] = changedRom;
    if (m_reverseLookup.count(changedRom.sha1) == 0)
      return;
    if (playTimeChanged) {
      for (const _RomListModelRow *rowPtr :
           m_reverseLookup.at(changedRom.sha1)) {
        const _RomListModelRow &changedRow = *rowPtr;
        const QModelIndex leftIndex =
            createIndex(changedRow[0].row, COL_LAST_PLAYED,
                        (void *)&changedRow[COL_LAST_PLAYED]);
        const QModelIndex rightIndex =
            createIndex(changedRow[0].row, COL_PLAY_TIME,
                        (void *)&changedRow[COL_PLAY_TIME]);
        const QVector<int> changedRoles{Qt::DisplayRole};
        emit dataChanged(leftIndex, rightIndex, changedRoles);
      }
    }
    if (nameChanged) {
      for (const _RomListModelRow *rowPtr :
           m_reverseLookup.at(changedRom.sha1)) {
        const _RomListModelRow &changedRow = *rowPtr;
        const QModelIndex index = createIndex(changedRow[0].row, COL_NAME,
                                              (void *)&changedRow[COL_NAME]);
        const QVector<int> changedRoles{Qt::DisplayRole};
        emit dataChanged(index, index, changedRoles);
      }
    }
  }
}

static constexpr int64 SECONDS = 1000;
static constexpr int64 MINUTES = 60 * SECONDS;
static constexpr int64 HOURS = 60 * MINUTES;

static inline string formatTimeSpan(int64 timeSpan) {
  const int64 hours = timeSpan / HOURS;
  const int64 minutes = (timeSpan % HOURS) / MINUTES;
  const int64 seconds = (timeSpan % MINUTES) / SECONDS;

  string timeString;
  timeString.reserve(16);

  if (hours > 0) {
    timeString += Number::toString(hours);
    timeString += "h ";
  }

  if (minutes > 0 || hours > 0) {
    timeString += Number::toString(minutes);
    timeString += "m ";
  }

  timeString += Number::toString(seconds);
  timeString += 's';

  return timeString;
}

QVariant RomListModel::data(const QModelIndex &index, int role) const {
  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  const Index *i = static_cast<const Index *>(index.internalPointer());
  if (i == nullptr)
    return QVariant();

  if (i->row < 0) {
    const QString label = GroupName::localize(m_groups.at(i->group).name);
    return QVariant::fromValue<QString>(label);
  }

  ConstRomReference rom = tryGetRom(index);
  switch (i->col) {
  case COL_NAME: {
    return QVariant::fromValue<QString>(getName(rom));
  }
  case COL_INTERNAL_NAME:
    return QVariant::fromValue<QString>(
        QString::fromUtf8(rom.info->internalName.c_str()));
  case COL_PATH:
    return QVariant::fromValue<QString>(
        QString::fromUtf8(rom.file->path.u8string().c_str()));
  case COL_LAST_PLAYED:
    if (rom.info->lastPlayed == 0) {
      return QVariant::fromValue<QString>("");
    }
    return QVariant::fromValue<QString>(
        QDateTime::fromMSecsSinceEpoch(rom.info->lastPlayed)
            .toString("yyyy-MM-dd h:mm AP"));
  case COL_PLAY_TIME:
    return QVariant::fromValue<QString>(
        formatTimeSpan(rom.info->playTime).c_str());
  default:
    return QVariant();
  }
}

Qt::ItemFlags RomListModel::flags(const QModelIndex &index) const {
  const Index *i = static_cast<const Index *>(index.internalPointer());
  if (i == nullptr)
    return Qt::NoItemFlags;

  if (i->row < 0) {
    if (i->col != 0)
      return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

QVariant RomListModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
  if (orientation == Qt::Vertical)
    return QVariant();
  if (role != Qt::DisplayRole)
    return QAbstractItemModel::headerData(section, orientation, role);
  switch (section) {
  case COL_NAME:
    return QVariant::fromValue<QString>(tr("Name"));
  case COL_INTERNAL_NAME:
    return QVariant::fromValue<QString>(tr("Internal Name"));
  case COL_PATH:
    return QVariant::fromValue<QString>(tr("File Path"));
  case COL_LAST_PLAYED:
    return QVariant::fromValue<QString>(tr("Last Played"));
  case COL_PLAY_TIME:
    return QVariant::fromValue<QString>(tr("Play Time"));
  default:
    return QVariant();
  }
}

QModelIndex RomListModel::index(int row, int column,
                                const QModelIndex &parent) const {
  if (m_groups.empty())
    return QModelIndex();
  if (parent == QModelIndex()) {
    if (column != 0)
      return QModelIndex();
    return createIndex(row, column, (void *)&m_groups.at(row).self);
  }
  const Index *i = static_cast<const Index *>(parent.internalPointer());
  if (i == nullptr || i->row >= 0)
    return QModelIndex();
  return createIndex(
      row, column, (void *)&m_groups.at(i->group).children.at(row).at(column));
}

QModelIndex RomListModel::parent(const QModelIndex &index) const {
  if (m_groups.empty())
    return QModelIndex();
  const Index *i = static_cast<const Index *>(index.internalPointer());
  if (i == nullptr || i->row < 0)
    return QModelIndex();
  const Index *parentIndex = &m_groups.at(i->group).self;
  return createIndex(parentIndex->group, 0, (void *)parentIndex);
}

bool RomListModel::hasChildren(const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return !m_groups.empty();
  }

  const Index *i = static_cast<const Index *>(parent.internalPointer());
  return i != nullptr && i->row < 0;
}

int RomListModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid()) {
    return (int)m_groups.size();
  }

  const Index *i = static_cast<const Index *>(parent.internalPointer());
  if (i == nullptr || i->row >= 0)
    return 0;
  return (int)m_groups.at(i->group).children.size();
}

void RomListModel::sortInternal() {
  m_reverseLookup.clear();
  for (Group &group : m_groups) {
    std::stable_sort(
        group.children.begin(), group.children.end(),
        [&](const _RomListModelRow &a, const _RomListModelRow &b) {
          RomFile &romFileA = m_romFiles.at(a[0].lookupIndex);
          RomFile &romFileB = m_romFiles.at(b[0].lookupIndex);
          const ConstRomReference romA =
              ConstRomReference{&romFileA, &m_romInfo.at(romFileA.sha1),
                                tryGetRhdcHackWithSha1(romFileA.sha1)};
          const ConstRomReference romB =
              ConstRomReference{&romFileB, &m_romInfo.at(romFileB.sha1),
                                tryGetRhdcHackWithSha1(romFileB.sha1)};
          const bool reverse = (m_sortOrder == Qt::DescendingOrder);
          return s_romComparers[m_sortBy](reverse ? romB : romA,
                                          reverse ? romA : romB);
        });

    int i = 0;
    for (_RomListModelRow &child : group.children) {
      const string &sha1 = m_romFiles.at(child[0].lookupIndex).sha1;
      m_reverseLookup[sha1].push_back(&child);
      for (Index &index : child) {
        index.row = i;
      }
      i++;
    }
  }
}

void RomListModel::sort(int column, Qt::SortOrder order) {
  m_sortBy = column;
  m_sortOrder = order;
  beginResetModel();
  sortInternal();
  endResetModel();
}

string RomListModel::tryGetGroup(const QModelIndex &index) const {
  const Index *i = static_cast<const Index *>(index.internalPointer());
  return (i == nullptr) ? string() : m_groups.at(i->group).name;
}

RomReference RomListModel::tryGetRom(const QModelIndex &index) {
  const Index *i = static_cast<const Index *>(index.internalPointer());
  if (i == nullptr || i->lookupIndex < 0)
    return {nullptr, nullptr, nullptr};
  RomFile *filePtr = &m_romFiles.at(i->lookupIndex);
  return {filePtr, &m_romInfo.at(filePtr->sha1),
          tryGetRhdcHackWithSha1(filePtr->sha1)};
}

ConstRomReference RomListModel::tryGetRom(const QModelIndex &index) const {
  const Index *i = static_cast<const Index *>(index.internalPointer());
  if (i == nullptr || i->lookupIndex < 0)
    return {nullptr, nullptr, nullptr};
  const RomFile *filePtr = &m_romFiles.at(i->lookupIndex);
  return {filePtr, &m_romInfo.at(filePtr->sha1),
          tryGetRhdcHackWithSha1(filePtr->sha1)};
}

QModelIndex RomListModel::tryGetIndex(const string &group,
                                      const fs::path &rom) const {
  for (const Group &g : m_groups) {
    if (g.name != group)
      continue;
    for (const _RomListModelRow &i : g.children) {
      if (m_romFiles.at(i[0].lookupIndex).path == rom) {
        return createIndex(i[0].row, 0, (void *)&i);
      }
    }
  }

  return QModelIndex();
}

QModelIndex RomListModel::tryGetIndex(const string &group) const {
  for (const Group &g : m_groups) {
    if (g.name == group) {
      return createIndex(g.self.group, 0, (void *)&g.self);
    }
  }

  return QModelIndex();
}

RomInfo *RomListModel::tryGetRomInfo(const string &sha1) {
  auto i = m_romInfo.find(sha1);
  return (i == m_romInfo.end()) ? nullptr : &i->second;
}

BasicRhdcHack *RomListModel::tryGetRhdcHackWithSha1(const string &sha1) {
  auto join = m_rhdcMap.find(sha1);
  if (join == m_rhdcMap.end())
    return nullptr;
  auto rhdc = m_rhdcInfo.find(join->second);
  if (rhdc == m_rhdcInfo.end())
    return nullptr;
  return &rhdc->second;
}

const BasicRhdcHack *
RomListModel::tryGetRhdcHackWithSha1(const string &sha1) const {
  const auto join = m_rhdcMap.find(sha1);
  if (join == m_rhdcMap.end())
    return nullptr;
  const auto rhdc = m_rhdcInfo.find(join->second);
  if (rhdc == m_rhdcInfo.end())
    return nullptr;
  return &rhdc->second;
}

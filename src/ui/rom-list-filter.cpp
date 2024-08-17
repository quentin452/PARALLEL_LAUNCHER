#include "src/ui/rom-list-filter.hpp"

#include "src/ui/rom-list-model.hpp"

static inline bool matches(const QString &filter, const std::string &text) {
  return QString(text.c_str()).contains(filter, Qt::CaseInsensitive);
}

bool RomListFilter::filterAcceptsRow(int sourceRow,
                                     const QModelIndex &sourceParent) const {
  if (m_filter.isEmpty())
    return true;

  const RomListModel *model = static_cast<const RomListModel *>(sourceModel());
  const QModelIndex index = model->index(sourceRow, 0, sourceParent);
  const ConstRomReference rom = model->tryGetRom(index);

  if (rom.file != nullptr &&
      matches(m_filter, rom.file->path.stem().u8string())) {
    return true;
  }

  if (rom.info != nullptr && matches(m_filter, rom.info->name)) {
    return true;
  }

  if (rom.rhdc != nullptr && matches(m_filter, rom.rhdc->info.name)) {
    return true;
  }

  return false;
}

void RomListFilter::fullUpdate() {
  static_cast<RomListModel *>(sourceModel())->fullUpdate();
}

void RomListFilter::rhdcUpdate() {
  static_cast<RomListModel *>(sourceModel())->rhdcUpdate();
}

void RomListFilter::partialUpdate(const RomInfo &changedRom, bool nameChanged,
                                  bool playTimeChanged) {
  static_cast<RomListModel *>(sourceModel())
      ->partialUpdate(changedRom, nameChanged, playTimeChanged);
}

string RomListFilter::tryGetGroup(const QModelIndex &index) const {
  return static_cast<const RomListModel *>(sourceModel())
      ->tryGetGroup(mapToSource(index));
}

RomReference RomListFilter::tryGetRom(const QModelIndex &index) {
  return static_cast<RomListModel *>(sourceModel())
      ->tryGetRom(mapToSource(index));
}

ConstRomReference RomListFilter::tryGetRom(const QModelIndex &index) const {
  return static_cast<const RomListModel *>(sourceModel())
      ->tryGetRom(mapToSource(index));
}

QModelIndex RomListFilter::tryGetIndex(const string &group,
                                       const fs::path &rom) const {
  return mapFromSource(static_cast<const RomListModel *>(sourceModel())
                           ->tryGetIndex(group, rom));
}

QModelIndex RomListFilter::tryGetIndex(const string &group) const {
  return mapFromSource(
      static_cast<const RomListModel *>(sourceModel())->tryGetIndex(group));
}

RomInfo *RomListFilter::tryGetRomInfo(const string &sha1) {
  return static_cast<RomListModel *>(sourceModel())->tryGetRomInfo(sha1);
}

bool RomListFilter::hasRoms() const noexcept {
  return static_cast<RomListModel *>(sourceModel())->hasRoms();
}

QVariant RomListFilter::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole && index.isValid() && !index.parent().isValid()) {
    const QVariant value = QSortFilterProxyModel::data(index, role);
    QString groupName = value.toString();
    if (groupName.isEmpty())
      return value;

    groupName.append(" (");
    groupName.append(Number::toString(rowCount(index)).c_str());
    groupName.append(')');

    return QVariant(groupName);
  }

  return QSortFilterProxyModel::data(index, role);
}

QVariant RomListFilter::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  return static_cast<const RomListModel *>(sourceModel())
      ->headerData(section, orientation, role);
}

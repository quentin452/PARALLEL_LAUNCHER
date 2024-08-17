#include "src/ui/rom-list-view.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/special-groups.hpp"
#include "src/db/data-provider.hpp"
#include "src/polyfill/crash.hpp"
#include "src/polyfill/file-explorer.hpp"
#include "src/polyfill/process.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/rhdc/ui/rhdc-layout-tester-dialog.hpp"
#include "src/rhdc/ui/rhdc-rating-dialog.hpp"
#include "src/rhdc/ui/rhdc-save-editor.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/import-save-dialog.hpp"
#include "src/ui/rom-list-filter.hpp"
#include "src/ui/rom-list-model.hpp"
#include <QHeaderView>
#include <QImage>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>

RomListRenderer::RomListRenderer(QAbstractItemDelegate *innerDelegate)
    : QAbstractItemDelegate(innerDelegate->parent()), m_inner(innerDelegate) {
  pl_assert(parent() != nullptr);
}

static QPixmap makeRhdcLinkIcon(QRgb colour) {
  QImage icon = Icon::rhdcLink().pixmap(16, 16).toImage().convertToFormat(
      QImage::Format_ARGB32);
  uint *pixels = (uint *)icon.bits();
  for (qsizetype i = 0; i < icon.sizeInBytes() / 4; i++) {
    pixels[i] = (pixels[i] & 0xFF000000) | (colour & 0x00FFFFFF);
  }
  return QPixmap::fromImage(icon);
}

static const QPixmap &getRhdcLinkIcon(QRgb colour) {
  static HashMap<QRgb, QPixmap> iconMap;
  const auto i = iconMap.find(colour);
  if (i == iconMap.end()) {
    iconMap[colour] = makeRhdcLinkIcon(colour);
    return iconMap.at(colour);
  } else {
    return i->second;
  }
}

static inline const QPixmap &getRhdcLinkIcon(const QPalette &palette,
                                             bool selected, bool focused,
                                             bool disabled) {
  return getRhdcLinkIcon(
      palette
          .color(disabled ? QPalette::Disabled
                          : ((focused || !selected) ? QPalette::Active
                                                    : QPalette::Inactive),
                 selected ? QPalette::HighlightedText : QPalette::Text)
          .rgb());
}

void RomListRenderer::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const {
  const QTreeView *treeView = dynamic_cast<const QTreeView *>(parent());
  pl_assert(treeView != nullptr);

  if (index.isValid() && !index.parent().isValid()) {
    QStyleOptionViewItem newOption = option;
    newOption.font.setBold(true);

    int width = 0;
    const int numColumns = treeView->model()->columnCount();
    for (int i = 0; i < numColumns; i++) {
      width += treeView->columnWidth(i);
    }

    newOption.rect.setWidth(width);
    m_inner->paint(painter, newOption, index);
  } else {
    const ConstRomReference rom =
        ((const RomListFilter *)index.model())->tryGetRom(index);
    if (rom.rhdc == nullptr || index.column() > 0) {
      m_inner->paint(painter, option, index);
    } else {
      QStyleOptionViewItem newOption = option;
      QRect &region = newOption.rect;

      if (treeView->currentIndex() == index) {
        painter->fillRect(
            QRect(region.topLeft(), region.bottomLeft() + QPoint(20, 0)),
            treeView->palette().color(treeView->hasFocus() ? QPalette::Active
                                                           : QPalette::Inactive,
                                      QPalette::Highlight));
      }

      const QPixmap &icon = getRhdcLinkIcon(
          treeView->palette(), treeView->currentIndex() == index,
          treeView->hasFocus(), !rom.file->local);
      painter->drawPixmap(region.left() + 1,
                          region.top() + (region.height() - 16) / 2, icon);

      region.setLeft(region.left() + 20);
      m_inner->paint(painter, newOption, index);
    }
  }
}

QSize RomListRenderer::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {
  QSize size = m_inner->sizeHint(option, index);
  if (index.isValid() && !index.parent().isValid()) {
    size.setWidth(0);
  }
  return size;
}

RomListView::RomListView(QWidget *parent) : QTreeView(parent) {
  setModel((QAbstractItemModel *)new RomListFilter(new RomListModel(this)));
  setContextMenuPolicy(Qt::CustomContextMenu);
  setItemDelegate(new RomListRenderer(itemDelegate()));
  setFocusPolicy(Qt::StrongFocus);
  setLayoutDirection(Qt::LeftToRight);

  header()->setSectionResizeMode(QHeaderView::Stretch);
  header()->setSectionResizeMode(0, QHeaderView::Stretch);
  header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(2, QHeaderView::Interactive);
  header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
  header()->setSectionsMovable(false);
  header()->setStretchLastSection(false);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this,
          SLOT(onRightClick(const QPoint &)));
  connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this,
          SLOT(onDoubleClick(const QModelIndex &)));
}

RomListView::~RomListView() { model()->deleteLater(); }

void RomListView::refetchAll() {
  TreeUiState uiState = saveTreeState();
  ((RomListFilter *)model())->fullUpdate();
  restoreTreeState(uiState);
}

void RomListView::refetchRhdc() {
  TreeUiState uiState = saveTreeState();
  ((RomListFilter *)model())->rhdcUpdate();
  restoreTreeState(uiState);
}

void RomListView::updateRomInfo(const RomInfo &romInfo, bool nameChanged,
                                bool playTimeChanged) {
  TreeUiState uiState = saveTreeState();
  ((RomListFilter *)model())
      ->partialUpdate(romInfo, nameChanged, playTimeChanged);
  restoreTreeState(uiState);
}

RomReference RomListView::tryGetSelectedRom() const {
  return ((RomListFilter *)model())->tryGetRom(currentIndex());
}

RomInfo *RomListView::tryGetRomInfo(const string &sha1) const {
  return ((RomListFilter *)model())->tryGetRomInfo(sha1);
}

bool RomListView::hasRoms() const {
  return ((const RomListFilter *)model())->hasRoms();
}

bool RomListView::selectRom(const string &group, const fs::path &rom) {
  const QModelIndex index =
      ((const RomListFilter *)model())->tryGetIndex(group, rom);
  const QModelIndex groupIndex =
      ((const RomListFilter *)model())->tryGetIndex(group);
  if (index.isValid()) {
    const bool groupExpanded = isExpanded(groupIndex);
    setCurrentIndex(index);
    if (!groupExpanded)
      collapse(groupIndex);
    return true;
  }
  return false;
}

static constexpr char ACTION_DELETE_SAVE = 1;
static constexpr char ACTION_EDIT_SAVE = 2;
static constexpr char ACTION_ADD_GROUP = 3;
static constexpr char ACTION_CREATE_GROUP = 4;
static constexpr char ACTION_REMOVE_GROUP = 5;
static constexpr char ACTION_OPEN_FOLDER = 6;
static constexpr char ACTION_DELETE_ROM = 7;
static constexpr char ACTION_RENAME_ROM = 8;
static constexpr char ACTION_REVERT_NAME = 9;
static constexpr char ACTION_RATE_HACK = 10;
static constexpr char ACTION_MARK_COMPLETED = 11;
static constexpr char ACTION_MARK_INCOMPLETE = 12;
static constexpr char ACTION_DOWNLOAD = 13;
static constexpr char ACTION_IMPORT_SAVE = 14;
static constexpr char ACTION_TEST_LAYOUT = 15;
static constexpr char ACTION_SHOW_SAVE = 16;

static inline QVariant serializeAction(char actionType) {
  char idStr[] = {actionType, '\0'};
  return QVariant::fromValue<QString>(QString(idStr));
}

static QVariant serializeAction(char actionType, const string &additonalData) {
  char idStr[] = {actionType, '\0'};
  return QVariant::fromValue<QString>(
      QString(idStr).append(additonalData.c_str()));
}

static inline bool shouldSyncGroup(const RomReference &rom,
                                   const string &group) {
  return (rom.rhdc != nullptr && !group.empty() &&
          (group == SpecialGroups::Favourites || group[0] != (char)1));
}

static inline bool canManuallyAddToGroup(const RomReference &rom,
                                         const string &group) {
  return rom.rhdc == nullptr || (group != SpecialGroups::WantToPlay &&
                                 group != SpecialGroups::InProgress &&
                                 group != SpecialGroups::Completed);
}

void RomListView::onRightClick(const QPoint &point) {
  const QModelIndex romEntry = indexAt(point);
  if (!romEntry.isValid())
    return;
  this->setCurrentIndex(romEntry);

  RomReference rom = tryGetSelectedRom();
  if (rom.file == nullptr)
    return;

  const std::vector<string> allGroups = DataProvider::fetchAllGroups();
  const fs::path saveFilePath =
      rom.file->local ? RetroArch::getSaveFilePath(rom.file->path) : fs::path();
  const string group =
      ((const RomListFilter *)model())->tryGetGroup(romEntry.parent());

  QMenu contextMenu(this);
  if (!saveFilePath.empty()) {
    if (fs::isRegularFileSafe(saveFilePath)) {
      contextMenu.addAction(tr("Delete Save File"))
          ->setData(serializeAction(ACTION_DELETE_SAVE));
      contextMenu.addAction(tr("[SM64] Edit Save File"))
          ->setData(serializeAction(ACTION_EDIT_SAVE));
    }

    contextMenu.addAction(tr("Import Project64 Save File"))
        ->setData(serializeAction(ACTION_IMPORT_SAVE));
    contextMenu.addAction(tr("Show Save File"))
        ->setData(serializeAction(ACTION_SHOW_SAVE));
    contextMenu.addSeparator();
  }

  if (!rom.file->local) {
    contextMenu.addAction(tr("Download"))
        ->setData(serializeAction(ACTION_DOWNLOAD));
  }

  bool existingGroupOptions = false;
  QMenu *tagMenu = contextMenu.addMenu(tr("Add to..."));
  for (const string &romGroup : allGroups) {
    if (romGroup == group || !canManuallyAddToGroup(rom, romGroup))
      continue;
    existingGroupOptions = true;
    tagMenu->addAction(GroupName::localize(romGroup))
        ->setData(serializeAction(ACTION_ADD_GROUP, romGroup));
  }
  if (existingGroupOptions)
    tagMenu->addSeparator();
  tagMenu->addAction(tr("New Group"))
      ->setData(serializeAction(ACTION_CREATE_GROUP));

  if (!group.empty() && canManuallyAddToGroup(rom, group)) {
    contextMenu.addAction(tr("Remove from %1").arg(GroupName::localize(group)))
        ->setData(serializeAction(ACTION_REMOVE_GROUP));
  }

  contextMenu.addSeparator();
  if (rom.file->local) {
    contextMenu.addAction(tr("Open Containing Folder"))
        ->setData(serializeAction(ACTION_OPEN_FOLDER));
    contextMenu.addAction(tr("Delete ROM"))
        ->setData(serializeAction(ACTION_DELETE_ROM));
  }
  contextMenu.addAction(tr("Rename"))
      ->setData(serializeAction(ACTION_RENAME_ROM));
  if (!rom.info->name.empty()) {
    contextMenu.addAction(tr("Revert ROM Name"))
        ->setData(serializeAction(ACTION_REVERT_NAME));
  }

  if (rom.rhdc != nullptr) {
    contextMenu.addSeparator();
    contextMenu.addAction(tr("Rate Hack"))
        ->setData(serializeAction(ACTION_RATE_HACK));
    if (rom.rhdc->info.starCount == 0) {
      if (rom.rhdc->progress.completed) {
        contextMenu.addAction(tr("Mark as Not Completed"))
            ->setData(serializeAction(ACTION_MARK_INCOMPLETE));
      } else {
        contextMenu.addAction(tr("Mark as Completed"))
            ->setData(serializeAction(ACTION_MARK_COMPLETED));
      }
    }
  }

  if (rom.file->local &&
      (RhdcCredentials::exists() || DataProvider::hasRhdcData())) {
    contextMenu.addSeparator();
    contextMenu.addAction("[RHDC] Test Star Layout")
        ->setData(serializeAction(ACTION_TEST_LAYOUT));
  }

  const fs::path romFilePath = rom.file->path;
  const QAction *action = contextMenu.exec(mapToGlobal(point));
  if (action != nullptr && action->data().type() == QVariant::String) {

    // Verify that the item hasn't changed since the menu opened
    RomReference rom = tryGetSelectedRom();
    if (rom.file == nullptr || rom.file->path != romFilePath) {
      if (!selectRom(group, romFilePath))
        return;
      rom = tryGetSelectedRom();
    }

    switch (action->data().value<QString>().toStdString().at(0)) {
    case ACTION_DELETE_SAVE: {
      if (QMessageBox::question(
              nullptr, tr("Confirm Delete"),
              tr("Are you sure you want to delete your save file?")) ==
          QMessageBox::Yes) {
        fs::error_code err;
        fs::remove(saveFilePath, err);
      }
      break;
    }
    case ACTION_SHOW_SAVE: {
      FileExplorer::showFile(saveFilePath);
      break;
    }
    case ACTION_ADD_GROUP: {
      const string groupName =
          action->data().value<QString>().toStdString().substr(1);
      DataProvider::addToGroup(rom.file->sha1, groupName);
      if (shouldSyncGroup(rom, groupName)) {
        RhdcApi::addHackToListAsync(
            rom.rhdc->info.hackId, GroupName::getRhdcName(groupName), []() {},
            RhdcApi::logApiError("Failed to add hack to list"));
      }
      refetchAll();
      break;
    }
    case ACTION_CREATE_GROUP: {
      const QString tagString = QInputDialog::getText(
          nullptr, tr("Create Group"), tr("Enter a name for your new group"));
      if (tagString.isEmpty() || tagString.isNull())
        return;

      const string groupName = tagString.toStdString();
      for (const string &g : allGroups) {
        if (groupName != g)
          continue;
        QMessageBox::critical(nullptr, tr("Group Exists"),
                              tr("A group with this name already exists"));
        return;
      }

      DataProvider::addGroup(groupName);
      DataProvider::addToGroup(rom.file->sha1, groupName);
      if (shouldSyncGroup(rom, groupName)) {
        RhdcApi::addHackToListAsync(
            rom.rhdc->info.hackId, GroupName::getRhdcName(groupName), []() {},
            RhdcApi::logApiError("Failed to add hack to list"));
      }
      refetchAll();
      break;
    }
    case ACTION_EDIT_SAVE: {
      if (rom.rhdc != nullptr && StarLayout::hasLayout(rom.rhdc->info.hackId)) {
        RhdcSaveEditor::exec(saveFilePath, rom.rhdc->info.hackId);
      } else {
        emit editSave(saveFilePath);
      }
      break;
    }
    case ACTION_REMOVE_GROUP: {
      DataProvider::removeFromGroup(rom.file->sha1, group);
      if (shouldSyncGroup(rom, group)) {
        RhdcApi::removeHackFromListAsync(
            rom.rhdc->info.hackId, GroupName::getRhdcName(group), []() {},
            RhdcApi::logApiError("Failed to remove hack from list"));
      }
      refetchAll();
      break;
    }
    case ACTION_OPEN_FOLDER: {
      FileExplorer::showFile(rom.file->path);
      break;
    }
    case ACTION_DELETE_ROM: {
      if (QMessageBox::question(
              nullptr, tr("Confirm Delete"),
              tr("This will completely remove the ROM file from your computer. "
                 "Are you sure you want to delete this ROM?")) ==
          QMessageBox::Yes) {
        DataProvider::deleteRomPath(rom.file->path);
        fs::error_code err;
        fs::remove(rom.file->path, err);
        refetchAll();
      }
      break;
    }
    case ACTION_RENAME_ROM: {
      const string name =
          QInputDialog::getText(nullptr, tr("Rename"),
                                tr("Enter a new name for your rom"))
              .toStdString();
      if (name.empty())
        return;
      rom.info->name = name;
      DataProvider::renameRom(rom.file->sha1, name);
      updateRomInfo(*rom.info, true, false);
      break;
    }
    case ACTION_REVERT_NAME: {
      rom.info->name.clear();
      DataProvider::renameRom(rom.file->sha1, "");
      updateRomInfo(*rom.info, true, false);
      break;
    }
    case ACTION_RATE_HACK: {
      RhdcRatingDialog dialog;
      dialog.setRatings(rom.rhdc->progress.rating,
                        rom.rhdc->progress.difficulty, rom.rhdc->info.isKaizo);
      if (dialog.exec() == QDialog::Accepted) {
        rom.rhdc->progress.rating = dialog.getQuality();
        rom.rhdc->progress.difficulty = dialog.getDifficulty();
        DataProvider::updateRhdcHackRating(rom.rhdc->info.hackId,
                                           rom.rhdc->progress.rating,
                                           rom.rhdc->progress.difficulty);
        RhdcApi::submitRatingsAsync(
            rom.rhdc->info.hackId, rom.rhdc->progress.rating,
            rom.rhdc->progress.difficulty, []() {},
            RhdcApi::logApiError("Failed to submit hack ratings"));
      }
      break;
    }
    case ACTION_MARK_INCOMPLETE: {
      rom.rhdc->progress.completed = false;
      DataProvider::removeFromGroup(rom.info->sha1, SpecialGroups::Completed);
      if (rom.info->playTime >= 5 * 60 * 1000) {
        DataProvider::addToGroup(rom.info->sha1, SpecialGroups::InProgress);
      } else {
        DataProvider::addToGroup(rom.info->sha1, SpecialGroups::WantToPlay);
      }
      DataProvider::updateRhdcStarProgress(rom.rhdc->info.hackId, 0, false);
      if (rom.rhdc->info.submitStarpower) {
        RhdcApi::submitCompletionAsync(
            rom.rhdc->info.hackId, false, []() {},
            RhdcApi::logApiError("Failed to submit hack progress"));
      }
      refetchAll();
      break;
    }
    case ACTION_MARK_COMPLETED: {
      rom.rhdc->progress.completed = true;
      DataProvider::removeFromGroup(rom.info->sha1, SpecialGroups::WantToPlay);
      DataProvider::removeFromGroup(rom.info->sha1, SpecialGroups::InProgress);
      DataProvider::addToGroup(rom.info->sha1, SpecialGroups::Completed);
      DataProvider::updateRhdcStarProgress(rom.rhdc->info.hackId, 0, true);
      if (rom.rhdc->info.submitStarpower) {
        RhdcApi::submitCompletionAsync(
            rom.rhdc->info.hackId, true, []() {},
            RhdcApi::logApiError("Failed to submit hack progress"));
      }
      refetchAll();
      break;
    }
    case ACTION_DOWNLOAD: {
      emit downloadPatch();
      break;
    }
    case ACTION_IMPORT_SAVE: {
      ImportSaveDialog::exec(saveFilePath);
      break;
    }
    case ACTION_TEST_LAYOUT: {
      window()->hide();
      RhdcLayoutTesterDialog *dialog =
          new RhdcLayoutTesterDialog(rom.file->path);
      connect(dialog, &RhdcLayoutTesterDialog::finished, this,
              [this](int) { this->window()->show(); });
      dialog->show();
      break;
    }
    default:
      break;
    }
  }
}

void RomListView::onDoubleClick(const QModelIndex &index) {
  const RomReference rom = ((RomListFilter *)model())->tryGetRom(index);
  if (rom.file != nullptr) {
    if (rom.file->local) {
      emit launchRom();
    } else {
      emit downloadPatch();
    }
  }
}

void RomListView::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    onDoubleClick(currentIndex());
  }
  QTreeView::keyPressEvent(event);
}

TreeUiState RomListView::saveTreeState() {
  TreeUiState uiState;

  const RomReference selectedRom = tryGetSelectedRom();
  if (selectedRom.file != nullptr) {
    uiState.selectedRom = selectedRom.file->path;
    uiState.selectedGroup =
        ((const RomListFilter *)model())->tryGetGroup(currentIndex().parent());
  }

  const int numGroups = model()->rowCount();
  for (int i = 0; i < numGroups; i++) {
    const QModelIndex index = model()->index(i, 0);
    if (!index.isValid())
      continue;

    const string groupName =
        ((const RomListFilter *)model())->tryGetGroup(index);
    if (isExpanded(index)) {
      m_expanded.insert(groupName);
    } else {
      m_expanded.erase(groupName);
    }
  }

  uiState.expandedGroups =
      std::vector<string>(m_expanded.begin(), m_expanded.end());
  return uiState;
}

void RomListView::restoreTreeState(const TreeUiState &state) {
  for (const string &group : state.expandedGroups) {
    const QModelIndex index =
        ((const RomListFilter *)model())->tryGetIndex(group);
    if (index.isValid()) {
      expand(index);
    }
  }

  if (!state.selectedRom.empty()) {
    selectRom(state.selectedGroup, state.selectedRom);
  }
}

void RomListView::search(QString search) {
  const bool hadSelection = currentIndex().isValid();

  const TreeUiState state = saveTreeState();
  ((RomListFilter *)model())->setFilter(search);
  restoreTreeState(state);

  // For some reason, this gets reset. Don't know why
  const RomInfoColumn visibleColumns =
      FileController::loadAppSettings().visibleColumns;
  setColumnHidden(0, false);
  setColumnHidden(1, !Flags::has(visibleColumns, RomInfoColumn::InternalName));
  setColumnHidden(2, !Flags::has(visibleColumns, RomInfoColumn::Path));
  setColumnHidden(3, !Flags::has(visibleColumns, RomInfoColumn::LastPlayed));
  setColumnHidden(4, !Flags::has(visibleColumns, RomInfoColumn::PlayTime));

  header()->setSectionResizeMode(QHeaderView::Stretch);
  header()->setSectionResizeMode(0, QHeaderView::Stretch);
  header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(2, QHeaderView::Interactive);
  header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
  header()->setSectionsMovable(false);
  header()->setStretchLastSection(false);

  if (hadSelection && !currentIndex().isValid()) {
    emit selectionFilteredOut();
  }
}

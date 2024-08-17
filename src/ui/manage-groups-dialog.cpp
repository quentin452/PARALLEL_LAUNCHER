#include "src/ui/manage-groups-dialog.hpp"
#include "ui_manage-groups-dialog.h"

#include "src/core/file-controller.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/special-groups.hpp"
#include "src/db/data-provider.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include <QInputDialog>
#include <QMessageBox>

ManageGroupsDialog::ManageGroupsDialog(QWidget *parent)
    : QDialog(parent), m_ui(new Ui::ManageGroupsDialog) {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());
  UiUtil::scaleWindow(this);
  UiUtil::fixDialogButtonsOnWindows(this);

  m_ui->addButton->setIcon(Icon::add());
  m_ui->renameButton->setIcon(Icon::edit());
  m_ui->deleteButton->setIcon(Icon::delet());
}

ManageGroupsDialog::~ManageGroupsDialog() { delete m_ui; }

static QString formatGroup(const string &groupName, int romCount) {
  QString nameAndCount = GroupName::localize(groupName);
  nameAndCount.append(" (");
  nameAndCount.append(Number::toString(romCount).c_str());
  nameAndCount.append(')');
  return nameAndCount;
}

void ManageGroupsDialog::updateButtons() {
  const bool groupSelected =
      (m_ui->groupsList->count() > 0 || m_ui->groupsList->currentRow() >= 0);
  m_ui->deleteButton->setEnabled(groupSelected);
  m_ui->renameButton->setEnabled(groupSelected);
}

void ManageGroupsDialog::deleteGroup() {
  QListWidgetItem *item = m_ui->groupsList->currentItem();
  if (item == nullptr) {
    updateButtons();
    return;
  }

  _MGD_GroupInfo itemData = item->data(Qt::UserRole).value<_MGD_GroupInfo>();

  if (itemData.romCount > 0) {
    if (QMessageBox::question(
            this, tr("Confirm Delete"),
            tr("Are you sure you want to delete this group?")) !=
        QMessageBox::Yes) {
      return;
    }
  }

  m_groups.erase(itemData.name);
  m_ui->groupsList->removeItemWidget(item);
  delete item;

  DataProvider::deleteGroup(itemData.name);
  updateButtons();
}

void ManageGroupsDialog::renameGroup() {
  QListWidgetItem *item = m_ui->groupsList->currentItem();
  if (item == nullptr) {
    updateButtons();
    return;
  }

  const QString qName = QInputDialog::getText(
      this, tr("Rename Group"), tr("Enter a new name for your group"));
  if (qName.isEmpty() || qName.isNull())
    return;

  _MGD_GroupInfo itemData = item->data(Qt::UserRole).value<_MGD_GroupInfo>();

  const string oldName = itemData.name;
  const string newName = qName.toStdString();

  if (m_groups.count(newName) > 0) {
    QMessageBox::critical(this, tr("Rename Failed"),
                          tr("A group with this name already exists."));
    return;
  }

  itemData.name = newName;
  item->setData(Qt::UserRole, QVariant::fromValue<_MGD_GroupInfo>(itemData));
  item->setText(formatGroup(newName, itemData.romCount));

  m_groups.erase(oldName);
  m_groups.insert(newName);
  DataProvider::renameGroup(oldName, newName);
}

void ManageGroupsDialog::addGroup() {
  const QString qName = QInputDialog::getText(
      this, tr("Create Group"), tr("Enter a name for your new group"));
  if (qName.isEmpty() || qName.isNull())
    return;

  const string group = qName.toStdString();
  if (m_groups.count(group) > 0) {
    QMessageBox::critical(this, tr("Create Failed"),
                          tr("A group with this name already exists."));
    return;
  }

  m_groups.insert(group);
  DataProvider::addGroup(group);

  QListWidgetItem *item = new QListWidgetItem(formatGroup(group, 0));
  item->setData(Qt::UserRole, QVariant::fromValue<_MGD_GroupInfo>({group, 0}));
  m_ui->groupsList->addItem(item);
  m_ui->groupsList->setCurrentItem(item);
  updateButtons();
}

void ManageGroupsDialog::showEvent(QShowEvent *event) {
  m_groups = DataProvider::fetchPersistentGroups();

  HashMap<string, int> groupSizes;
  groupSizes.reserve(m_groups.size());
  for (const string &group : m_groups) {
    groupSizes[group] = DataProvider::countRomsInGroup(group);
  }

  m_ui->groupsList->clear();
  for (const string &group : m_groups) {
    QListWidgetItem *item =
        new QListWidgetItem(formatGroup(group, groupSizes[group]));
    item->setData(Qt::UserRole, QVariant::fromValue<_MGD_GroupInfo>(
                                    {group, groupSizes[group]}));
    m_ui->groupsList->addItem(item);
  }

  if (!m_groups.empty()) {
    m_ui->groupsList->setCurrentRow(0);
  }

  updateButtons();
  QDialog::showEvent(event);
}

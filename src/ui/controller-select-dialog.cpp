#include "src/ui/controller-select-dialog.hpp"
#include "ui_controller-select-dialog.h"

#include "src/core/file-controller.hpp"
#include "src/core/logging.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/input/gamepad-controller.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/crash.hpp"
#include "src/polyfill/file-explorer.hpp"
#include "src/ui/controller-config-dialog.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"
#include <QMessageBox>
#include <map>
#ifdef _WIN32
#include "src/core/filesystem.hpp"
#include "src/polyfill/env.hpp"
#endif

static inline InputDriver getInputDriver() {
  return FileController::loadAppSettings().inputDriver;
}

static inline QString getInputDriverErrorMessage() {
#ifdef _WIN32
  if (getInputDriver() == InputDriver::SDL &&
      !fs::existsSafe(BaseDir::program() /
                      L"parallel-launcher-sdl-relay.exe")) {
    return ControllerSelectDialog::tr(
        "Could not get connected controllers because a file "
        "(<tt>parallel-launcher-sdl-relay.exe</tt>) is missing from your "
        "Parallel Launcher installation. "
        "Please re-install Parallel Launcher to restore the missing file(s). "
        "If this problem persists, you may need to whitelist the "
        "<a href=\"#\">Parallel Launcher installation directory</a> in your "
        "anti-virus software.");
  }
#endif
  return ControllerSelectDialog::tr(
      "Input driver failed to initialize due to an unexpected error.");
}

ControllerSelectDialog::ControllerSelectDialog(QWidget *parent)
    : QDialog(parent), m_ui(new Ui::ControllerSelectDialog),
      m_disableEvents(true) {
  m_ui->setupUi(this);
  UiUtil::scaleWindow(this);
  UiUtil::fixDialogButtonsOnWindows(this);
  m_ui->errorBubble->setVisible(false);

  const InputDriver inputDriver = getInputDriver();
  m_profiles = FileController::loadControllerProfiles(inputDriver);
  m_profileMap = FileController::loadControllerMappings(inputDriver);

  const std::map<InputDriver, QString> driverNames = {
      {InputDriver::HID, QString("HID")},
      {InputDriver::SDL, QString("SDL2")},
      {InputDriver::DirectInput, QString("DirectInput 8")},
      {InputDriver::JoyDev, QString("JoyDev (linuxraw)")}};

  int selectedIndex = -1;
  for (InputDriver driver : InputDriverUtil::getSupported()) {
    if (driver == inputDriver) {
      selectedIndex = m_ui->driverSelectBox->count();
    }

    m_ui->driverSelectBox->addItem(driverNames.at(driver),
                                   QVariant::fromValue<int>((int)driver));
  }

  m_ui->driverSelectBox->setCurrentIndex(selectedIndex);
  m_ui->driverSelection->setVisible(m_ui->driverSelectBox->count() > 1);

#ifdef _WIN32
  const bool rawInput = FileController::loadAppSettings().sdlRawInput;
  m_ui->rawInputCheckbox->setVisible(inputDriver == InputDriver::SDL);
  m_ui->rawInputCheckbox->setChecked(rawInput);
  Environment::set("SDL_JOYSTICK_RAWINPUT", rawInput ? "1" : "0");
#else
  m_ui->rawInputCheckbox->setVisible(false);
#endif

  setWindowIcon(Icon::appIcon());
  m_ui->editButton->setIcon(Icon::edit());
  m_ui->deleteButton->setIcon(Icon::delet());

  for (const auto &i : m_profiles) {
    m_ui->profileList->addItem(i.first.c_str());
  }

  UiUtil::fixFontSizeOnMac(m_ui->label_3);

  connect(&GamepadController::instance(),
          SIGNAL(gamepadConnected(GamepadConnectedEvent)), this,
          SLOT(controllerConnected(GamepadConnectedEvent)));
  connect(&GamepadController::instance(),
          SIGNAL(gamepadDisconnected(GamepadDisconnectedEvent)), this,
          SLOT(controllerDisconnected(GamepadDisconnectedEvent)));

  m_disableEvents = false;
  try {
    GamepadController::instance().start();
    m_ui->errorBubble->setVisible(false);
  } catch (const std::exception &ex) {
    m_ui->errorBubble->setText(getInputDriverErrorMessage());
    m_ui->errorBubble->setVisible(true);
    logError("Failed to start input driver: "s + ex.what());
  }
  refreshDeviceList();
}

ControllerSelectDialog::~ControllerSelectDialog() {
  GamepadController::instance().stop();
  delete m_ui;
}

void ControllerSelectDialog::refreshDeviceList() {
  m_disableEvents = true;

  GamepadId lastSelected = m_ui->deviceList->currentRow();
  int selectedRow = 0;

  m_ui->deviceList->clear();
  for (const auto &i : m_devices) {
    QListWidgetItem *item = new QListWidgetItem;
    item->setData(Qt::UserRole, QVariant::fromValue<int>(i.first));

    if (!i.second.name.empty()) {
      item->setText(i.second.name.c_str());
    } else {
      item->setText(("Unknown ("s + i.second.uuid.toString() + ')').c_str());
    }

    m_ui->deviceList->addItem(item);
    if (i.first == lastSelected) {
      selectedRow = m_ui->deviceList->count() - 1;
    }
  }

  if (!m_devices.empty()) {
    m_ui->deviceList->setCurrentRow(selectedRow);
  }

  m_disableEvents = false;
  deviceSelected();
}

void ControllerSelectDialog::deviceSelected() {
  if (m_disableEvents)
    return;

  if (m_devices.empty() || m_ui->deviceList->currentItem() == nullptr) {
    m_ui->profileList->setEnabled(false);
    m_ui->editButton->setEnabled(false);
    m_ui->deleteButton->setEnabled(false);
    return;
  }

  m_ui->profileList->setEnabled(true);
  m_ui->editButton->setEnabled(true);

  GamepadId id = m_ui->deviceList->currentItem()->data(Qt::UserRole).toInt();
  const ControllerInfo &info = m_devices.at(id);

  string activeProfile;
  if (m_profileMap.count(info.uuid)) {
    activeProfile = m_profileMap.at(info.uuid);
    if (m_profiles.count(activeProfile) <= 0) {
      activeProfile.clear();
    }
  }

  if (activeProfile.empty()) {
    activeProfile = DefaultProfile::get(getInputDriver(),
                                        getControllerType(info.controllerId))
                        .name;
  }

  if (DefaultProfile::exists(getInputDriver(), activeProfile)) {
    m_ui->editButton->setIcon(Icon::add());
    m_ui->editButton->setText(tr("New Profile"));
    m_ui->deleteButton->setEnabled(false);
  } else {
    m_ui->editButton->setIcon(Icon::edit());
    m_ui->editButton->setText(tr("Edit Profile"));
    m_ui->deleteButton->setEnabled(true);
  }

  m_disableEvents = true;
  for (int i = 0; i < m_ui->profileList->count(); i++) {
    if (m_ui->profileList->item(i)->text().toStdString() == activeProfile) {
      m_ui->profileList->setCurrentRow(i);
      break;
    }
  }

  if (m_ui->profileList->currentItem() == nullptr) {
    m_ui->profileList->setCurrentRow(0);
  }

  m_disableEvents = false;
}

void ControllerSelectDialog::profileSelected() {
  if (m_ui->deviceList->currentItem() == nullptr ||
      m_ui->profileList->currentItem() == nullptr) {
    m_ui->editButton->setEnabled(false);
    m_ui->deleteButton->setEnabled(false);
    return;
  }

  if (m_disableEvents)
    return;
  m_ui->editButton->setEnabled(true);

  const GamepadId id =
      m_ui->deviceList->currentItem()->data(Qt::UserRole).toInt();
  if (m_devices.count(id) <= 0)
    return;

  const Uuid &uuid = m_devices.at(id).uuid;
  const string profile = m_ui->profileList->currentItem()->text().toStdString();
  m_profileMap[uuid] = profile;

  if (DefaultProfile::exists(getInputDriver(), profile)) {
    m_ui->editButton->setIcon(Icon::add());
    m_ui->editButton->setText(tr("New Profile"));
    m_ui->deleteButton->setEnabled(false);
  } else {
    m_ui->editButton->setIcon(Icon::edit());
    m_ui->editButton->setText(tr("Edit Profile"));
    m_ui->deleteButton->setEnabled(true);
  }

  FileController::saveControllerMappings(getInputDriver(), m_profileMap);
}

void ControllerSelectDialog::editProfile() {
  pl_assert(m_ui->deviceList->currentItem() != nullptr);
  pl_assert(m_ui->profileList->count() > 0);

  if (!m_ui->profileList->currentItem()) {
    m_ui->profileList->setCurrentRow(0);
    pl_assert(m_ui->profileList->currentItem() != nullptr);
  }

  const GamepadId deviceId =
      m_ui->deviceList->currentItem()->data(Qt::UserRole).toInt();
  const ControllerInfo &info = m_devices.at(deviceId);

  ControllerConfigDialog dialog;
  dialog.setActiveController(
      info.name, info.uuid, deviceId,
      m_profiles.at(m_ui->profileList->currentItem()->text().toStdString()));
  dialog.exec();

  m_profiles = FileController::loadControllerProfiles(getInputDriver());
  const string &activeProfile = dialog.getProfileName();
  int profileIndex = 0;
  int i = 0;

  m_disableEvents = true;
  m_ui->profileList->clear();
  for (const auto &p : m_profiles) {
    m_ui->profileList->addItem(p.first.c_str());
    if (p.first == activeProfile) {
      profileIndex = i;
    }
    i++;
  }

  m_ui->profileList->setCurrentRow(profileIndex);
  m_disableEvents = false;
  profileSelected();
}

void ControllerSelectDialog::deleteProfile() {
  pl_assert(m_ui->profileList->currentItem() != nullptr);

  const string profile = m_ui->profileList->currentItem()->text().toStdString();
  pl_assert(!DefaultProfile::exists(getInputDriver(), profile));

  if (QMessageBox::question(
          nullptr, tr("Confirm Delete"),
          tr("Are you sure you want to delete controller profile '%1'?")
              .arg(profile.c_str())) != QMessageBox::Yes) {
    return;
  }

  m_profiles.erase(profile);
  FileController::saveControllerProfiles(getInputDriver(), m_profiles);
  delete m_ui->profileList->takeItem(m_ui->profileList->currentRow());

  m_ui->profileList->setCurrentRow(0);
  profileSelected();
}

void ControllerSelectDialog::driverChanged() {
  if (m_disableEvents)
    return;

  const InputDriver driver =
      (InputDriver)m_ui->driverSelectBox->currentData(Qt::UserRole).toInt();

  AppSettings settings = FileController::loadAppSettings();
  settings.inputDriver = driver;
#ifdef _WIN32
  m_ui->rawInputCheckbox->setVisible(driver == InputDriver::SDL);
  m_ui->rawInputCheckbox->setChecked(settings.sdlRawInput);
  Environment::set("SDL_JOYSTICK_RAWINPUT",
                   FileController::loadAppSettings().sdlRawInput ? "1" : "0");
#endif
  FileController::saveAppSettings(settings);

  m_ui->deviceList->clear();
  m_devices.clear();

  try {
    GamepadController::instance().setInputDriver(driver);
    m_ui->errorBubble->setVisible(false);
  } catch (std::exception &ex) {
    m_ui->errorBubble->setText(getInputDriverErrorMessage());
    m_ui->errorBubble->setVisible(true);
    logError("Failed to start input driver: "s + ex.what());
  }

  m_disableEvents = true;
  m_ui->profileList->clear();

  m_profiles = FileController::loadControllerProfiles(driver);
  m_profileMap = FileController::loadControllerMappings(driver);

  for (const auto &i : m_profiles) {
    m_ui->profileList->addItem(i.first.c_str());
  }

  m_ui->profileList->setCurrentRow(0);
  m_disableEvents = false;

  deviceSelected();
  profileSelected();
}

void ControllerSelectDialog::rawInputChanged() {
#ifdef _WIN32
  AppSettings settings = FileController::loadAppSettings();
  settings.sdlRawInput = m_ui->rawInputCheckbox->isChecked();
  Environment::set("SDL_JOYSTICK_RAWINPUT",
                   FileController::loadAppSettings().sdlRawInput ? "1" : "0");
  FileController::saveAppSettings(settings);
  driverChanged();
#endif
}

void ControllerSelectDialog::controllerConnected(GamepadConnectedEvent event) {
  m_devices[event.id] = event.info;
  refreshDeviceList();
}

void ControllerSelectDialog::controllerDisconnected(
    GamepadDisconnectedEvent event) {
  m_devices.erase(event.id);
  refreshDeviceList();
}

void ControllerSelectDialog::openInstallDirectory() {
  FileExplorer::showFolder(BaseDir::program());
}

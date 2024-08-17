#include "src/ui/multiplayer-controller-select-dialog.hpp"
#include "ui_multiplayer-controller-select-dialog.h"

#include "src/ui/get-controller-port-dialog.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

MultiplayerControllerSelectDialog::MultiplayerControllerSelectDialog(
    bool multiPort)
    : QDialog(nullptr), m_ui(new Ui::MultiplayerControllerSelectDialog) {
  m_ui->setupUi(this);
  setWindowIcon(Icon::appIcon());
  UiUtil::scaleWindow(this);
  UiUtil::fixDialogButtonsOnWindows(this);

  connect(&GamepadController::instance(),
          SIGNAL(gamepadConnected(GamepadConnectedEvent)), this,
          SLOT(gamepadConnected(GamepadConnectedEvent)));
  connect(&GamepadController::instance(),
          SIGNAL(gamepadDisconnected(GamepadDisconnectedEvent)), this,
          SLOT(gamepadDisconnected(GamepadDisconnectedEvent)));
  GamepadController::instance().start();

  if (multiPort) {
    if (m_ui->device1->count() > 1)
      m_ui->device1->setCurrentIndex(1);
    if (m_ui->device1->count() > 2)
      m_ui->device3->setCurrentIndex(2);
    m_ui->device2->setEnabled(false);
    m_ui->device4->setEnabled(false);
  } else {
    if (m_ui->device1->count() > 1)
      m_ui->device1->setCurrentIndex(1);
    if (m_ui->device1->count() > 2)
      m_ui->device2->setCurrentIndex(2);
    if (m_ui->device1->count() > 3)
      m_ui->device3->setCurrentIndex(3);
    if (m_ui->device1->count() > 4)
      m_ui->device4->setCurrentIndex(4);
  }
}

MultiplayerControllerSelectDialog::~MultiplayerControllerSelectDialog() {
  GamepadController::instance().stop();
  delete m_ui;
}

void MultiplayerControllerSelectDialog::gamepadConnected(
    GamepadConnectedEvent event) {
  m_devices[event.id] = {event.id, std::move(event.info)};
  updateComboBoxes();
}

void MultiplayerControllerSelectDialog::gamepadDisconnected(
    GamepadDisconnectedEvent event) {
  m_devices.erase(event.id);
  updateComboBoxes();
}

static void
updateComboBox(QComboBox *comboBox,
               const std::map<GamepadId, ConnectedGamepad> &devices) {
  GamepadId gamepadId = -1;

  QVariant id = comboBox->currentData(Qt::UserRole);
  if (!id.isNull())
    gamepadId = id.value<int>();

  comboBox->clear();
  comboBox->addItem("-- None --");
  for (const auto &i : devices) {
    const ControllerInfo &info = i.second.info;
    const string name = info.name.empty() ? info.uuid.toString() : info.name;

    comboBox->addItem(name.c_str(), QVariant::fromValue<int>(i.first));
  }

  comboBox->setCurrentIndex(0);
  if (gamepadId < 0)
    return;

  for (int i = 0; i < comboBox->count(); i++) {
    if (comboBox->itemData(i, Qt::UserRole) == gamepadId) {
      comboBox->setCurrentIndex(i);
      break;
    }
  }
}

void MultiplayerControllerSelectDialog::updateComboBoxes() {
  updateComboBox(m_ui->device1, m_devices);
  updateComboBox(m_ui->device2, m_devices);
  updateComboBox(m_ui->device3, m_devices);
  updateComboBox(m_ui->device4, m_devices);
}

static ConnectedGamepad
getPortController(QComboBox *comboBox,
                  const std::map<GamepadId, ConnectedGamepad> &devices) {
  QVariant id = comboBox->currentData(Qt::UserRole);
  if (id.isNull()) {
    return {-1, ControllerInfo()};
  }

  const GamepadId gamepadId = id.value<int>();
  if (devices.count(gamepadId) > 0) {
    return devices.at(gamepadId);
  }

  return {-1, ControllerInfo()};
}

std::array<ConnectedGamepad, 4>
MultiplayerControllerSelectDialog::getControllers() const {
  std::array<ConnectedGamepad, 4> controllers;
  controllers[0] = getPortController(m_ui->device1, m_devices);
  controllers[1] = getPortController(m_ui->device2, m_devices);
  controllers[2] = getPortController(m_ui->device3, m_devices);
  controllers[3] = getPortController(m_ui->device4, m_devices);
  return controllers;
}

bool MultiplayerControllerSelectDialog::canBindSavestates() const {
  return m_ui->bindSavestateCheckbox->isChecked();
}
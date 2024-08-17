#ifndef SRC_UI_CONTROLLER_SELECT_DIALOG_HPP_
#define SRC_UI_CONTROLLER_SELECT_DIALOG_HPP_

#include "src/input/gamepad-controller.hpp"
#include <QDialog>
#include <map>
#include <unordered_map>

namespace Ui {
class ControllerSelectDialog;
}

class ControllerSelectDialog : public QDialog {
  Q_OBJECT

private:
  Ui::ControllerSelectDialog *m_ui;
  std::map<GamepadId, ControllerInfo> m_devices;
  std::map<string, ControllerProfile> m_profiles;
  HashMap<Uuid, string> m_profileMap;
  bool m_disableEvents;

  void refreshDeviceList();

public:
  explicit ControllerSelectDialog(QWidget *parent = nullptr);
  virtual ~ControllerSelectDialog();

private slots:
  void deviceSelected();
  void profileSelected();
  void editProfile();
  void deleteProfile();
  void driverChanged();
  void rawInputChanged();

  void controllerConnected(GamepadConnectedEvent event);
  void controllerDisconnected(GamepadDisconnectedEvent event);
  void openInstallDirectory();
};

#endif /* SRC_UI_CONTROLLER_SELECT_DIALOG_HPP_ */

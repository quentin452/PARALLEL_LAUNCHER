#ifndef SRC_UI_MULTIPLAYER_CONTROLLER_SELECT_DIALOG_HPP_
#define SRC_UI_MULTIPLAYER_CONTROLLER_SELECT_DIALOG_HPP_

#include "src/input/gamepad-controller.hpp"
#include <QComboBox>
#include <QDialog>
#include <array>

namespace Ui {
class MultiplayerControllerSelectDialog;
}

class MultiplayerControllerSelectDialog : public QDialog {
  Q_OBJECT

private:
  Ui::MultiplayerControllerSelectDialog *m_ui;
  std::map<GamepadId, ConnectedGamepad> m_devices;

  void updateComboBoxes();

public:
  MultiplayerControllerSelectDialog(bool multiPort);
  virtual ~MultiplayerControllerSelectDialog();

  std::array<ConnectedGamepad, 4> getControllers() const;
  bool canBindSavestates() const;

private slots:
  void gamepadConnected(GamepadConnectedEvent event);
  void gamepadDisconnected(GamepadDisconnectedEvent event);
};

#endif /* SRC_UI_MULTIPLAYER_CONTROLLER_SELECT_DIALOG_HPP_ */

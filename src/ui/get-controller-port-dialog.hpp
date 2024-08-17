#ifndef SRC_UI_GET_CONTROLLER_PORT_DIALOG_HPP_
#define SRC_UI_GET_CONTROLLER_PORT_DIALOG_HPP_

#include "src/input/gamepad-controller.hpp"
#include <QMessageBox>

class GetControllerPortDialog : public QMessageBox {
  Q_OBJECT

private:
  GamepadId m_gamepadId;

  GetControllerPortDialog();

public:
  virtual ~GetControllerPortDialog() {}

  static inline GamepadId getGamepadId() {
    GetControllerPortDialog dialog;
    dialog.exec();
    return dialog.m_gamepadId;
  }

private slots:
  void buttonPressed(GamepadButtonEvent event);
};

#endif /* SRC_UI_GET_CONTROLLER_PORT_DIALOG_HPP_ */

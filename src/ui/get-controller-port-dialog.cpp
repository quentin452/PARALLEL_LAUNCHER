#include "src/ui/get-controller-port-dialog.hpp"

#include "src/ui/icons.hpp"

GetControllerPortDialog::GetControllerPortDialog()
    : QMessageBox(
          QMessageBox::Information, tr("Choose Controller"),
          tr("Press any button on the controller to bind to this port."),
          QMessageBox::Cancel),
      m_gamepadId(-1) {
  using namespace Icon;
  setWindowIcon(appIcon());
  connect(&GamepadController::instance(),
          SIGNAL(gamepadButtonPressed(GamepadButtonEvent)), this,
          SLOT(buttonPressed(GamepadButtonEvent)));
}

void GetControllerPortDialog::buttonPressed(GamepadButtonEvent event) {
  if (event.isPressed) {
    m_gamepadId = event.id;
    accept();
  }
}

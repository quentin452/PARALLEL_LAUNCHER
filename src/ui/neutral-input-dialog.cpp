#include "src/ui/neutral-input-dialog.hpp"
#include "ui_neutral-input-dialog.h"

#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

NeutralInputDialog::NeutralInputDialog(GamepadId gamepadId)
    : QDialog(nullptr), m_ui(new Ui::NeutralInputDialog),
      m_gamepadId(gamepadId) {
  m_ui->setupUi(this);
  UiUtil::scaleWindow(this);
  setWindowIcon(Icon::appIcon());
  connect(&GamepadController::instance(),
          SIGNAL(gamepadButtonPressed(GamepadButtonEvent)), this,
          SLOT(buttonPressed(GamepadButtonEvent)));
}

NeutralInputDialog::~NeutralInputDialog() { delete m_ui; }

void NeutralInputDialog::buttonPressed(GamepadButtonEvent event) {
  if (event.id == m_gamepadId && event.isPressed) {
    accept();
  }
}

#ifndef SRC_UI_SINGLEPLAYER_CONTROLLER_SELECT_DIALOG_HPP_
#define SRC_UI_SINGLEPLAYER_CONTROLLER_SELECT_DIALOG_HPP_

#include "src/input/gamepad-controller.hpp"
#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class SingleplayerControllerSelectDialog;
}

class SingleplayerControllerSelectDialog : public QDialog {
  Q_OBJECT

private:
  Ui::SingleplayerControllerSelectDialog *m_ui;
  ConnectedGamepad m_gamepad;

public:
  SingleplayerControllerSelectDialog();
  virtual ~SingleplayerControllerSelectDialog();

  inline ConnectedGamepad getSelectedController() const { return m_gamepad; }

protected:
  virtual void keyPressEvent(QKeyEvent *event) override;

private slots:
  void buttonPressed(GamepadButtonEvent event);
};

#endif /* SRC_UI_SINGLEPLAYER_CONTROLLER_SELECT_DIALOG_HPP_ */

#ifndef SRC_UI_NEUTRAL_INPUT_DIALOG_HPP_
#define SRC_UI_NEUTRAL_INPUT_DIALOG_HPP_

#include <QDialog>
#include "src/input/gamepad-controller.hpp"

namespace Ui {
	class NeutralInputDialog;
}

class NeutralInputDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::NeutralInputDialog *m_ui;
	const GamepadId m_gamepadId;

	public:
	explicit NeutralInputDialog( GamepadId gamepadId );
	virtual ~NeutralInputDialog();

	private slots:
	void buttonPressed( GamepadButtonEvent event );

};



#endif /* SRC_UI_NEUTRAL_INPUT_DIALOG_HPP_ */

#ifndef SRC_UI_BIND_INPUT_DIALOG_HPP_
#define SRC_UI_BIND_INPUT_DIALOG_HPP_

#include <QDialog>
#include <map>
#include "src/input/gamepad-controller.hpp"

namespace Ui {
	class BindInputDialog;
}

class BindInputDialog : protected QDialog {
	Q_OBJECT

	private:
	Ui::BindInputDialog *m_ui;
	GamepadId m_gamepadId;
	Binding m_binding;
	GamepadState m_initialState;
	std::map<ubyte,QTimer*> m_axisTimers;

	void cleanup();

	public:
	BindInputDialog( const QString &input, GamepadId gamepadId, bool showSkipButton );
	virtual ~BindInputDialog();

	Binding getBinding( bool &cancelled );
	inline Binding getBinding() {
		bool scratch;
		return getBinding( scratch );
	}

	private slots:
	void buttonPressed( GamepadButtonEvent event );
	void axisMoved( GamepadAxisEvent event );
	void hatMoved( GamepadHatEvent event );
	void axisHeld();

};


#endif /* SRC_UI_BIND_INPUT_DIALOG_HPP_ */

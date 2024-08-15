#ifndef SRC_UI_MULTIPLAYER_CONTROLLER_SELECT_DIALOG_HPP_
#define SRC_UI_MULTIPLAYER_CONTROLLER_SELECT_DIALOG_HPP_

#include <QDialog>
#include <QComboBox>
#include <array>
#include "src/input/gamepad-controller.hpp"

namespace Ui {
	class MultiplayerControllerSelectDialog;
}

class MultiplayerControllerSelectDialog : public QDialog {
	Q_OBJECT

	private:
	Ui::MultiplayerControllerSelectDialog *m_ui;
	std::map<GamepadId,ConnectedGamepad> m_devices;

	void updateComboBoxes();
	void detectDevice( QComboBox *comboBox );

	public:
	MultiplayerControllerSelectDialog( bool multiPort );
	virtual ~MultiplayerControllerSelectDialog();

	std::array<ConnectedGamepad,4> getControllers() const;
	bool canBindSavestates() const;

	private slots:
	void gamepadConnected( GamepadConnectedEvent event );
	void gamepadDisconnected( GamepadDisconnectedEvent event );

	void detectDevice1();
	void detectDevice2();
	void detectDevice3();
	void detectDevice4();

};


#endif /* SRC_UI_MULTIPLAYER_CONTROLLER_SELECT_DIALOG_HPP_ */

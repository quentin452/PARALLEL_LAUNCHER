#ifndef SRC_UI_INPUT_MODE_SELECT_HPP_
#define SRC_UI_INPUT_MODE_SELECT_HPP_

#include <QComboBox>
#include <map>
#include "src/core/controller.hpp"

class InputModeSelect : public QComboBox {
	Q_OBJECT

	private:
	std::map<Uuid,int> m_indexLookup;
	bool m_ignoreEvents;

	public:
	InputModeSelect( QWidget *parent = nullptr );
	~InputModeSelect() {}

	void refresh();
	void setSelected( const Uuid &id );
	const InputMode &getSelected() const;

	signals:
	void valueChanged();

	private slots:
	void selectionChangedInternal();

};



#endif /* SRC_UI_INPUT_MODE_SELECT_HPP_ */

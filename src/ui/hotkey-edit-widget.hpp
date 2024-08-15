#ifndef SRC_UI_HOTKEY_EDIT_WIDGET_HPP_
#define SRC_UI_HOTKEY_EDIT_WIDGET_HPP_

#include <QPushButton>
#include "src/input/keyboard.hpp"

class HotkeyEditWidget : public QPushButton {
	Q_OBJECT

	private:
	KeyId m_hotkey;
	bool m_active;

	public:
	HotkeyEditWidget( QWidget *parent = nullptr );
	virtual ~HotkeyEditWidget() {}

	inline KeyId value() const { return m_hotkey; }
	inline bool hasValue() const { return m_hotkey <= KeyId::LAST_VALID; }

	protected:
	virtual void focusOutEvent( QFocusEvent *event ) override;
	virtual void keyPressEvent( QKeyEvent *event ) override;
	virtual bool focusNextPrevChild( bool next ) override;

	public slots:
	void setValue( KeyId hotkey );
	void clear();

	private slots:
	void onClick();

	signals:
	void hotkeyChanged( KeyId );
};

namespace Ui {
	class HotkeyEditPanel;
}

class HotkeyEditPanel : public QWidget {
	Q_OBJECT

	private:
	Ui::HotkeyEditPanel *m_ui;

	public:
	HotkeyEditPanel( QWidget *parent = nullptr );
	virtual ~HotkeyEditPanel();

	KeyId value() const;
	inline bool hasValue() const { return value() <= KeyId::LAST_VALID; }

	public slots:
	void setValue( KeyId hotkey );
	void clear();

	signals:
	void hotkeyChanged( KeyId );
};


#endif /* SRC_UI_HOTKEY_EDIT_WIDGET_HPP_ */

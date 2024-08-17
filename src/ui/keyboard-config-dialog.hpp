#ifndef SRC_UI_KEYBOARD_CONFIG_DIALOG_HPP_
#define SRC_UI_KEYBOARD_CONFIG_DIALOG_HPP_

#include <QAbstractButton>
#include <QDialog>

namespace Ui {
class KeyboardConfigDialog;
}

class KeyboardConfigDialog : public QDialog {
  Q_OBJECT

private:
  Ui::KeyboardConfigDialog *m_ui;

public:
  KeyboardConfigDialog(QWidget *parent = nullptr);
  ~KeyboardConfigDialog();

protected:
  virtual void showEvent(QShowEvent *event) override;

private slots:
  void dialogButtonClicked(QAbstractButton *button);
};

#endif /* SRC_UI_KEYBOARD_CONFIG_DIALOG_HPP_ */

#ifndef SRC_UI_CONTROLLER_CONFIG_DIALOG_HPP_
#define SRC_UI_CONTROLLER_CONFIG_DIALOG_HPP_

#include "src/core/controller.hpp"
#include "src/core/uuid.hpp"
#include <QAbstractButton>
#include <QDialog>

namespace Ui {
class ControllerConfigDialog;
}

class ControllerConfigDialog : public QDialog {
  Q_OBJECT

private:
  Ui::ControllerConfigDialog *m_ui;
  ControllerProfile m_profile;
  Uuid m_controllerUuid;
  int m_controllerId;

public:
  explicit ControllerConfigDialog(QWidget *parent = nullptr);
  virtual ~ControllerConfigDialog();

  void setActiveController(const string &name, const Uuid &uuid, int id,
                           const ControllerProfile &profile);
  inline const string &getProfileName() const { return m_profile.name; }

private slots:
  void save();
  void saveAs();
  void bindAll();
  void bindSingle(QAbstractButton *button);
};

#endif /* SRC_UI_CONTROLLER_CONFIG_DIALOG_HPP_ */

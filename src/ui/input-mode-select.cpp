#include "src/ui/input-mode-select.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/preset-controllers.hpp"

InputModeSelect::InputModeSelect(QWidget *parent)
    : QComboBox(parent), m_ignoreEvents(false) {
  connect(this, SIGNAL(currentIndexChanged(int)), this,
          SLOT(selectionChangedInternal()));
  refresh();
}

void InputModeSelect::refresh() {
  const Uuid selected = getSelected().id;
  m_ignoreEvents = true;
  clear();
  int i = 0;
  for (const auto &m : FileController::loadInputModes()) {
    m_indexLookup[m.first] = i;
    addItem(tr(m.second.name.c_str()),
            QVariant::fromValue<QString>(tr(m.first.toString().c_str())));
    setItemData(i++,
                QVariant::fromValue<QString>(tr(m.second.description.c_str())),
                Qt::ToolTipRole);
  }
  setSelected(selected);
  m_ignoreEvents = false;
}

void InputModeSelect::setSelected(const Uuid &id) {
  m_ignoreEvents = true;
  const auto i = m_indexLookup.find(id);
  setCurrentIndex((i == m_indexLookup.end()) ? 0 : i->second);
  selectionChangedInternal();
  m_ignoreEvents = false;
}

const InputMode &InputModeSelect::getSelected() const {
  Uuid id;
  if (!Uuid::tryParse(currentData().toString().toStdString(), id)) {
    return DefaultInputModes::Normal;
  }

  const std::map<Uuid, InputMode> &inputModes =
      FileController::loadInputModes();
  const auto mode = inputModes.find(id);
  return (mode == inputModes.end()) ? DefaultInputModes::Normal : mode->second;
}

void InputModeSelect::selectionChangedInternal() {
  const InputMode &mode = getSelected();
  setToolTip(tr(mode.name.c_str()) + QString(": ") +
             tr(mode.description.c_str()));
  if (!m_ignoreEvents) {
    emit valueChanged();
  }
}

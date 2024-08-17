#include "src/rhdc/ui/star-bit-toggle.hpp"

#include <QKeyEvent>
#include <QMouseEvent>

StarBitToggle::StarBitToggle(QWidget *parent, const QPixmap &checkedImage,
                             const QPixmap &uncheckedImage, bool editable,
                             bool checked)
    : QLabel(parent), m_checkedImage(checkedImage),
      m_uncheckedImage(uncheckedImage), m_editable(editable),
      m_checked(checked) {
  setPixmap(checked ? checkedImage : uncheckedImage);
  setMinimumSize(QSize(24, 24));
  setScaledContents(true);
  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  if (editable)
    setFocusPolicy(Qt::StrongFocus);
}

void StarBitToggle::setChecked(bool checked) {
  if (checked == m_checked)
    return;
  m_checked = checked;
  setPixmap(checked ? m_checkedImage : m_uncheckedImage);
  emit toggled(checked);
}

void StarBitToggle::toggle() { setChecked(!m_checked); }

void StarBitToggle::mousePressEvent(QMouseEvent *event) {
  if (m_editable && (event->button() & Qt::LeftButton)) {
    toggle();
  }
  QLabel::mousePressEvent(event);
}

void StarBitToggle::keyPressEvent(QKeyEvent *event) {
  if (m_editable &&
      (event->key() == Qt::Key_Space || event->key() == Qt::Key_Enter)) {
    toggle();
  }
  QLabel::keyPressEvent(event);
}

QSize StarBitToggle::minimumSizeHint() const { return QSize(24, 24); }

QSize StarBitToggle::sizeHint() const { return QSize(24, 24); }

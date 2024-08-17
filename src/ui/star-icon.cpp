#include "src/ui/star-icon.hpp"

#include <QIcon>

static inline const QPixmap &imgBlueStar() {
  static const QPixmap s_img = QIcon(":/sm64/star-missing.png").pixmap(64, 64);
  return s_img;
}

static inline const QPixmap &imgYellowStar() {
  static const QPixmap s_img =
      QIcon(":/sm64/star-collected.png").pixmap(64, 64);
  return s_img;
}

static inline const QPixmap &imgCannonClosed() {
  static const QPixmap s_img =
      QIcon(":/sm64/cannon-inactive.svg").pixmap(64, 64);
  return s_img;
}

static inline const QPixmap &imgCannonOpen() {
  static const QPixmap s_img = QIcon(":/sm64/cannon-active.svg").pixmap(64, 64);
  return s_img;
}

StarIcon::StarIcon(QWidget *parent, ubyte courseId, ubyte starId,
                   bool collected)
    : QLabel(parent), m_courseId(courseId), m_starId(starId),
      m_collected(collected) {
  setCursor(Qt::PointingHandCursor);
  setCollected(collected);
}

void StarIcon::setCollected(bool collected) {
  m_collected = collected;
  if (isCannon()) {
    setPixmap(m_collected ? imgCannonOpen() : imgCannonClosed());
  } else {
    setPixmap(m_collected ? imgYellowStar() : imgBlueStar());
  }
}

void StarIcon::mousePressEvent(QMouseEvent *ev) {
  setCollected(!m_collected);
  QLabel::mousePressEvent(ev);
}

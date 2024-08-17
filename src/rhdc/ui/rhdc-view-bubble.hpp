#ifndef SRC_RHDC_UI_RHDC_VIEW_BUBBLE_HPP_
#define SRC_RHDC_UI_RHDC_VIEW_BUBBLE_HPP_

#include <QPushButton>
#include <QTimer>
#include <QWidget>

namespace Ui {
class RhdcViewBubble;
}

class RhdcViewBubble : public QWidget {
  Q_OBJECT

private:
  Ui::RhdcViewBubble *const m_ui;
  const QPushButton *const m_target;
  QTimer m_timer;

public:
  explicit RhdcViewBubble(QWidget *parent, QPushButton *target);
  ~RhdcViewBubble();

public slots:
  void dismiss();

private slots:
  void reposition();
};

#endif /* SRC_RHDC_UI_RHDC_VIEW_BUBBLE_HPP_ */

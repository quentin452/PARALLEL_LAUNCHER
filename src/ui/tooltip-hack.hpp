#ifndef SRC_UI_TOOLTIP_HACK_HPP_
#define SRC_UI_TOOLTIP_HACK_HPP_

#include <QObject>

class TooltipHack : public QObject {
  Q_OBJECT

public:
  TooltipHack(QObject *parent = nullptr) : QObject(parent) {}
  ~TooltipHack() {}

  bool eventFilter(QObject *object, QEvent *event) override;
};

#endif /* SRC_UI_TOOLTIP_HACK_HPP_ */

#ifndef SRC_UI_UI_FIXES_HPP_
#define SRC_UI_UI_FIXES_HPP_

#include <QApplication>
#include <QEvent>

#ifndef __linux__
class WindowsIconFixer : public QObject {
  Q_OBJECT

public:
  WindowsIconFixer(QObject *parent = nullptr) : QObject(parent) {}
  ~WindowsIconFixer() {}

  bool eventFilter(QObject *object, QEvent *event) override;
};
#endif

class RtlLabelFixer : public QObject {
  Q_OBJECT

public:
  RtlLabelFixer(QObject *parent = nullptr) : QObject(parent) {}
  ~RtlLabelFixer() {}

  bool eventFilter(QObject *object, QEvent *event) override;
};

void applyUiFixes(QApplication &app);

#endif /* SRC_UI_UI_FIXES_HPP_ */

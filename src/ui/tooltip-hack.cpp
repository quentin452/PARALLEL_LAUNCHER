#include "src/ui/tooltip-hack.hpp"

#include <QEvent>
#include <QWidget>

thread_local bool selfTriggered = false;

bool TooltipHack::eventFilter(QObject *object, QEvent *event) {
  if (selfTriggered)
    return true;

  QWidget *widget = dynamic_cast<QWidget *>(object);
  if (widget == nullptr || event->type() != QEvent::ToolTipChange) {
    return QObject::eventFilter(object, event);
  }

  static const QChar LT = QChar('<');
  static const QChar GT = QChar('>');
  static const QChar AMP = QChar('&');

  QString tooltip = widget->toolTip();
  for (int i = 0; i < tooltip.length(); i++) {
    if (tooltip[i] == LT) {
      tooltip[i] = AMP;
      tooltip.insert(++i, "lt;");
      i += 2;
    } else if (tooltip[i] == GT) {
      tooltip[i] = AMP;
      tooltip.insert(++i, "gt;");
      i += 2;
    } else if (tooltip[i] == AMP) {
      tooltip.insert(++i, "amp;");
      i += 3;
    }
  }

  tooltip.prepend("<font>");
  tooltip.append("</font>");

  selfTriggered = true;
  widget->setToolTip(tooltip);
  selfTriggered = false;
  return true;
}

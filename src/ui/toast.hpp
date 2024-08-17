#ifndef SRC_UI_TOAST_HPP_
#define SRC_UI_TOAST_HPP_

#include <QMainWindow>
#include <QString>

enum class ToastType { Info, Warn, Error };

namespace ToastMessageManager {

extern void setWindow(QMainWindow *window);
extern void display(ToastType type, const QString &message);

}; // namespace ToastMessageManager

#endif /* SRC_UI_TOAST_HPP_ */

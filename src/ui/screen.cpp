#include "src/ui/screen.hpp"

#include <QApplication>
#include <QGuiApplication>
#include <QWidget>
#include <QWindow>

QScreen *Screen::getActive() {
  const QWidget *const activeWindow = QApplication::activeWindow();
  if (activeWindow != nullptr) {
    const QWindow *const handle = activeWindow->windowHandle();
    if (handle != nullptr) {
      QScreen *screen = handle->screen();
      if (screen != nullptr)
        return screen;
    }
  }

  for (const QWidget *window : QApplication::topLevelWidgets()) {
    const QWindow *const handle = window->windowHandle();
    if (handle == nullptr)
      continue;

    QScreen *screen = handle->screen();
    if (screen != nullptr)
      return screen;
  }

  return QGuiApplication::primaryScreen();
}

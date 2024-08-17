#include "src/ui/now-playing-window.hpp"

#include "src/core/qthread.hpp"
#include "src/core/steam-deck.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/now-playing-widget.hpp"
#include "src/ui/screen.hpp"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QWindow>
#include <functional>
#include <memory>

class PersistentWindow : public QMainWindow {

private:
  bool m_forceClose;
  QPoint m_position;
  std::shared_ptr<bool> m_exists;
  const std::function<void(void)> m_sizedCallback;

public:
  PersistentWindow(const std::function<void(void)> &sizedCallback = []() {})
      : QMainWindow(nullptr), m_forceClose(false), m_exists(new bool(true)),
        m_sizedCallback(sizedCallback) {}
  virtual ~PersistentWindow() { *m_exists = false; }

  inline void forceClose() {
    m_forceClose = true;
    close();
    m_forceClose = false;
  }

protected:
  virtual void closeEvent(QCloseEvent *event) override {
    if (m_forceClose) {
      QMainWindow::closeEvent(event);
    } else {
      setCentralWidget(new QLabel(QString(), this));
      hide();
      event->ignore();
    }
  }

  virtual void setVisible(bool visible) override {
    if (!visible) {
      m_position = geometry().topLeft();
    } else {
      std::shared_ptr<bool> isAlive = m_exists;
      QtThread::safeAsync([this, isAlive]() {
        if (!*isAlive)
          return;

        const QRect workspace =
            QApplication::desktop()->availableGeometry(this);
        const QMargins margins = (this->windowHandle() != nullptr)
                                     ? this->windowHandle()->frameMargins()
                                     : QMargins(0, 0, 0, 0);

        QRect geo = this->geometry();
        if (this->m_position.isNull()) {
#ifdef _WIN32
          // Windows' window manager is too dumb to not put this behind the
          // RetroArch window every time.
          geo.moveTo(workspace.left() + margins.left(),
                     workspace.top() + margins.top());
#endif
        } else {
          geo.moveTo(this->m_position);
        }

        if (geo.right() > workspace.right() - margins.right()) {
          geo.moveRight(workspace.right() - margins.right());
        }

        if (geo.left() < workspace.left() + margins.left()) {
          geo.moveLeft(workspace.left() + margins.left());
        }

        if (geo.bottom() > workspace.bottom() - margins.bottom()) {
          geo.moveBottom(workspace.bottom() - margins.bottom());
        }

        if (geo.top() < workspace.top() + margins.top()) {
          geo.moveTop(workspace.top() + margins.top());
        }

        // Shrink to fit
        if (geo.width() > 300)
          geo.setWidth(300);
        if (geo.height() > 175)
          geo.setHeight(175);
        this->setGeometry(geo);

        this->m_sizedCallback();
      });
    }

    QMainWindow::setVisible(visible);
  }
};

static PersistentWindow *s_window = nullptr;
static bool s_reload = false;

static void onWindowSized() {
  if (s_window == nullptr)
    return;

  NowPlayingWidget *nowPlayingWidget =
      dynamic_cast<NowPlayingWidget *>(s_window->centralWidget());
  if (nowPlayingWidget == nullptr)
    return;

  StarDisplayWidget *starDisplay = nowPlayingWidget->starDisplay();
  if (starDisplay == nullptr)
    return;

  if (SteamDeck::isSteamDeck()) {
    int desktopWidth, desktopHeight;
    Screen::getDesktopSize(desktopWidth, desktopHeight);

    const int x = desktopHeight * 4 / 3;
    const QRect geo = QRect(x, 0, desktopWidth, desktopHeight);
    s_window->setMaximumWidth(desktopWidth - x);
    s_window->setGeometry(geo);
  }

  QRect geo = s_window->geometry();
  // TODO: figure out where this extra space is coming from
  geo.setWidth(geo.width() + 4);
  geo.setHeight(geo.height() + 4);

  const QRect workspace = QApplication::desktop()->availableGeometry(s_window);
  const QMargins margins = (s_window->windowHandle() != nullptr)
                               ? s_window->windowHandle()->frameMargins()
                               : QMargins(0, 0, 0, 0);

  bool requiresScrollbar = false;

  const int decorationWidth = margins.left() + margins.right();
  if (geo.width() + decorationWidth > workspace.width()) {
    geo.setLeft(workspace.left() + margins.left());
    geo.setWidth(workspace.width() - decorationWidth);
    requiresScrollbar = true;
  }

  const int decorationHeight = margins.top() + margins.bottom();
  if (geo.height() + decorationHeight > workspace.height()) {
    geo.setTop(workspace.top() + margins.top());
    geo.setHeight(workspace.height() - decorationHeight);
    requiresScrollbar = true;
  }

  if (requiresScrollbar && starDisplay != nullptr) {
    starDisplay->makeScrollable();

    QtThread::safeAsync([geo]() {
      if (s_window == nullptr)
        return;
      s_window->setMaximumWidth(geo.width());
      s_window->setMaximumHeight(geo.height());
      s_window->setGeometry(geo);
    });
  }

  s_window->setGeometry(geo);
}

void NowPlayingWindow::open(const fs::path &romPath, const string &sha1,
                            int64 pastPlayTime,
                            const StarLayout *layoutOverride) {
  s_reload = false;

  if (s_window == nullptr) {
    s_window = new PersistentWindow(onWindowSized);
    s_window->setWindowTitle(
        NowPlayingWidget::tr("Parallel Launcher - Now Playing"));
    s_window->setBaseSize(QSize(300, 175));
    s_window->setWindowIcon(Icon::appIcon());

    if (SteamDeck::isSteamDeck()) {
      s_window->setWindowFlag(Qt::FramelessWindowHint, true);
    }
  }

  NowPlayingWidget *widget =
      new NowPlayingWidget(s_window, romPath, sha1, pastPlayTime,
                           layoutOverride, &s_reload);

  if (SteamDeck::isSteamDeck() && widget->starDisplay() == nullptr) {
    s_window->close();
    return;
  }

  s_window->setCentralWidget(widget);
  s_window->show();
}

void NowPlayingWindow::close() {
  if (s_window == nullptr)
    return;

  QLabel *placeholder = new QLabel(QString(), s_window);
  s_window->setCentralWidget(placeholder);
  s_window->hide();
}

void NowPlayingWindow::free() {
  if (s_window == nullptr)
    return;

  close();
  s_window->forceClose();
  s_window->deleteLater();
  s_window = nullptr;
}

bool NowPlayingWindow::shouldReload() { return s_reload; }

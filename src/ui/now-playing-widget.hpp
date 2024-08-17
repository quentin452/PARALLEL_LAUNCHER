#ifndef SRC_UI_NOW_PLAYING_WIDGET_HPP_
#define SRC_UI_NOW_PLAYING_WIDGET_HPP_

#include "src/core/filesystem.hpp"
#include "src/polyfill/process.hpp"
#include "src/rhdc/core/layout.hpp"
#include "src/rhdc/ui/star-display-widget.hpp"
#include <QTimer>
#include <QWidget>

namespace Ui {
class NowPlayingWidget;
}

class NowPlayingWidget : public QWidget {
  Q_OBJECT

private:
  Ui::NowPlayingWidget *const m_ui;
  StarDisplayWidget *m_starDisplay;
  QTimer m_timer;
  const int64 m_pastPlayTime;
  const int64 m_startTime;

public:
  explicit NowPlayingWidget(QWidget *parent, const fs::path &romPath,
                            const string &sha1, int64 pastPlayTime,
                            const StarLayout *layoutOverride = nullptr,
                            bool *shouldReloadPtr = nullptr);
  virtual ~NowPlayingWidget();

  int64 getSessionTime() const;
  inline StarDisplayWidget *starDisplay() noexcept { return m_starDisplay; }

private slots:
  void updateTimers();
};

#endif /* SRC_UI_NOW_PLAYING_WIDGET_HPP_ */

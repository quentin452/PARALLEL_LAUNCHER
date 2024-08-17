#ifndef SRC_RHDC_UI_RHDC_HACK_WIDGET_HPP_
#define SRC_RHDC_UI_RHDC_HACK_WIDGET_HPP_

#include "src/rhdc/core/hack.hpp"
#include <QWidget>

namespace Ui {
class RhdcHackWidget;
}

class ThumbnailNotifier : public QObject {
  Q_OBJECT

public:
  ThumbnailNotifier() : QObject(nullptr) {}
  ~ThumbnailNotifier() {}

  void notifyDownloaded();

signals:
  void downloaded();
};

class RhdcHackWidget : public QWidget {
  Q_OBJECT

private:
  Ui::RhdcHackWidget *m_ui;
  const RhdcHackExt m_hack;
  const uint64 m_shuffleOrder;
  uint64 m_seededShuffleOrder;
  bool m_thumbnailLoaded;

  void loadThumbnail();
  void initialize(bool selected);

protected:
  virtual void paintEvent(QPaintEvent *event) override;

public:
  RhdcHackWidget(QWidget *parent, const RhdcHackExt &hack, uint64 shuffleSeed);

private:
  RhdcHackWidget(QWidget *parent, const RhdcHackExt &hack, bool selected,
                 uint64 shuffleOrderG, uint64 shuffleOrderS);

public:
  ~RhdcHackWidget();

  inline const RhdcHackExt &hack() const noexcept { return m_hack; }
  inline uint64 shuffleOrderG() const noexcept { return m_shuffleOrder; }
  inline uint64 shuffleOrderS() const noexcept { return m_seededShuffleOrder; }

  inline void reshuffle(uint64 seed) noexcept {
    m_seededShuffleOrder = (m_shuffleOrder ^ seed) * 14313749767032793493ull;
  }

  inline RhdcHackWidget *clone(QWidget *parent, bool selected) const {
    return new RhdcHackWidget(parent, m_hack, selected, m_shuffleOrder,
                              m_seededShuffleOrder);
  }

public slots:
  void editorResized(int width);
};

#endif /* SRC_RHDC_UI_RHDC_HACK_WIDGET_HPP_ */

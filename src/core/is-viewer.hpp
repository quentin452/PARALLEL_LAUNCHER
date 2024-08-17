#ifndef SRC_CORE_IS_VIEWER_HPP_
#define SRC_CORE_IS_VIEWER_HPP_

#include "src/polyfill/fifo-pipe.hpp"
#include <QString>
#include <QThread>
#include <string>

namespace __ISViewer_Private {

class ISViewerThread : public QThread {
  Q_OBJECT

private:
  ReadableFifoPipe *const m_pipe;

public:
  inline ISViewerThread(QObject *parent, ReadableFifoPipe *pipe)
      : QThread(parent), m_pipe(pipe) {}
  inline ~ISViewerThread() {}

protected:
  void run() override;

signals:
  void messageReceived(QString message);
  void error(bool connected);
};

} // namespace __ISViewer_Private

class ISViewer : public QObject {
  Q_OBJECT

private:
  QThread *m_thread;
  ReadableFifoPipe m_pipe;

public:
  int start();
  void stop();

  inline ISViewer(QObject *parent = nullptr)
      : QObject(parent), m_thread(nullptr), m_pipe() {}
  inline ~ISViewer() { stop(); }

  inline std::string getPipeFilePath() const noexcept {
    return m_pipe.filePath();
  }

signals:
  void messageReceived(QString msg);
  void error(bool connected);
};

#endif /* SRC_CORE_IS_VIEWER_HPP_ */

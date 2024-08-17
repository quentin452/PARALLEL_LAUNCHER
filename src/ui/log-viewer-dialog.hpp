#ifndef SRC_UI_LOG_VIEWER_DIALOG_HPP_
#define SRC_UI_LOG_VIEWER_DIALOG_HPP_

#include "src/core/logging.hpp"
#include <QDialog>
#include <QFrame>
#include <QLabel>

namespace Ui {
class LogViewerDialog;
}

class LogViewerEntry : public QFrame {
  Q_OBJECT

private:
  LogEntry m_entry;
  QLabel *m_timestampLabel;
  QLabel *m_messageLabel;

public:
  LogViewerEntry(QWidget *parent, LogEntry &&entry);
  ~LogViewerEntry();

  inline const LogEntry &entry() const { return m_entry; }

  inline void setTimestampVisible(bool isVisible) {
    m_timestampLabel->setVisible(isVisible);
  }
};

class LogViewerDialog : public QDialog {
  Q_OBJECT

private:
  Ui::LogViewerDialog *m_ui;

public:
  LogViewerDialog(QWidget *parent = nullptr);
  ~LogViewerDialog();

private slots:
  void updateLogVisibility();
};

#endif /* SRC_UI_LOG_VIEWER_DIALOG_HPP_ */

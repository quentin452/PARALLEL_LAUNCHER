#ifndef SRC_UI_ERROR_NOTIFIER_HPP_
#define SRC_UI_ERROR_NOTIFIER_HPP_

#include "src/types.hpp"
#include <QObject>
#include <QString>

class ErrorNotifier : public QObject {
  Q_OBJECT

private:
  ErrorNotifier();

private slots:
  void onAlert(QString message) const;

signals:
  void sendAlert(QString message);

private:
  static ErrorNotifier &instance();
  inline void alertImpl(const string &message) {
    emit sendAlert(message.c_str());
  }

public:
  ~ErrorNotifier() {}

  static inline void alert(const string &message) {
    instance().alertImpl(message);
  }
};

#endif /* SRC_UI_ERROR_NOTIFIER_HPP_ */

#include "src/ui/error-notifier.hpp"

#include <QMessageBox>

static const char *const s_errorPrefix = QT_TRANSLATE_NOOP("ErrorNotifier", "The application encountered an unexpected error:") "\n";

ErrorNotifier::ErrorNotifier() : QObject() {
    connect(this, SIGNAL(sendAlert(QString)), this, SLOT(onAlert(QString)), Qt::QueuedConnection);
}

void ErrorNotifier::onAlert(QString message) const {
    QMessageBox::critical(nullptr, tr("Unhandled Error"), message.prepend(tr(s_errorPrefix)));
}

ErrorNotifier &ErrorNotifier::instance() {
    static ErrorNotifier s_instance;
    return s_instance;
}

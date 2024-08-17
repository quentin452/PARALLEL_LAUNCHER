#include "src/core/app-context.hpp"

#include "src/core/time.hpp"
#include "src/core/version.hpp"
#include "src/polyfill/base-directory.hpp"
#include <QApplication>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include <QLocale>
#include <QNetworkInterface>
#include <QScreen>
#include <QSysInfo>

static AppContextInfo s_context = {nullptr, nullptr, 0, nullptr, nullptr,
                                   0,       0,       0, 0};

static inline int getInterfaceRelevance(const QNetworkInterface &interface) {
  if (!interface.isValid())
    return -1;
  if (interface.flags().testFlag(QNetworkInterface::IsLoopBack))
    return -1;

  switch (interface.type()) {
  case QNetworkInterface::Ethernet:
  case QNetworkInterface::Wifi:
    return (interface.name() == "eth0") ? 5 : 4;
  case QNetworkInterface::Virtual:
    return 2;
  case QNetworkInterface::Unknown:
    return 1;
  case QNetworkInterface::Loopback:
    return -1;
  default:
    return 3;
  }
}

static inline QString getMacAddress() {
  QNetworkInterface eth0 = QNetworkInterface::interfaceFromName("eth0");
  if (eth0.isValid() && eth0.type() != QNetworkInterface::Loopback) {
    return eth0.hardwareAddress();
  }

  QNetworkInterface bestMatch;
  int bestScore = 0;

  for (const QNetworkInterface &interface :
       QNetworkInterface::allInterfaces()) {
    const int relevanceScore = getInterfaceRelevance(interface);
    if (relevanceScore > bestScore) {
      bestMatch = interface;
      bestScore = relevanceScore;
    }
  }

  return bestMatch.isValid() ? bestMatch.hardwareAddress() : QString();
}

static string generateDeviceId() {
  QString id = getMacAddress();
  if (id.isEmpty()) {
#ifdef __linux__
    id = QSysInfo::machineUniqueId().toHex().toStdString().c_str();
#else
    id = QSysInfo::machineHostName();
#endif
  }

  const QByteArray hashedId =
      QCryptographicHash::hash(id.toUtf8(), QCryptographicHash::Sha3_256);
  return hashedId.toHex().toStdString();
}

const AppContextInfo &getAppContext() { return s_context; }

void initCoreAppContext() {
  static const string filePath =
      (BaseDir::data() / _NFS("crashlog")).u8string();
  static const string versionString = CurrentVersion::Application.toString();
  s_context.crashlogFilepath = filePath.c_str();
  s_context.appVersion = versionString.c_str();
  s_context.appStartTime = Time::now();
}

void initExtraAppContext() {
  static const string locale = QLocale().name().toStdString();
  static const string deviceId = generateDeviceId();
  const QSize screenSize = QApplication::primaryScreen()->size();
  s_context.deviceId = deviceId.c_str();
  s_context.screenWidth = screenSize.width();
  s_context.screenHeight = screenSize.height();
  s_context.screenDpiX = QApplication::desktop()->logicalDpiX();
  s_context.screenDpiY = QApplication::desktop()->logicalDpiY();
  s_context.locale = locale.c_str();
}

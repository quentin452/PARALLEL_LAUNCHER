#include "src/core/app-context.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/logging.hpp"
#include "src/core/retroarch.hpp"
#include "src/core/thread-safety.hpp"
#include "src/core/updates.hpp"
#include "src/core/zip.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/crash.hpp"
#include "src/rhdc/ui/rhdc-download-dialog.hpp"
#include "src/types.hpp"
#include "src/ui/core-installer.hpp"
#include "src/ui/direct-play.hpp"
#include "src/ui/main-window.hpp"
#include "src/ui/ui-fixes.hpp"
#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QGuiApplication>
#include <QMessageBox>
#include <QTranslator>
#include <cstdlib>
#include <initializer_list>
#include <set>
#ifdef _WIN32
#include "src/core/retroarch-sdl-fix.hpp"
#include "src/core/sdcard.hpp"
#include "src/polyfill/windows/locale.hpp"
#include "src/polyfill/windows/unicode.hpp"
#endif
#if !defined(__linux__)
#include "src/updaters/self-updater.hpp"
#include <QInputDialog>
#endif
#ifndef FLATPAK_VERSION
#include "src/updaters/retroarch-updater.hpp"
#endif

#include "src/core/sdcard.hpp"

static int runMainWindow(QApplication &app) {
  QGuiApplication::setQuitOnLastWindowClosed(false);
  MainWindow mainWindow;
  mainWindow.show();
  return app.exec();
}

static inline fs::path tryGetRomPath(int argc, [[maybe_unused]] char **argv) {
  if (argc < 2)
    return fs::path();

#ifdef _WIN32
  string romArg = Unicode::argvUtf8().at(1);
#else
  string romArg = argv[1];
#endif

  if (romArg.substr(0, 7) == "file://") {
    romArg = romArg.substr(7);
  }

  return fs::to_path(romArg);
}

static fs::path processRomPath(const fs::path &romPath) {
  if (romPath.u8string().substr(0, 7) == "rhdc://") {
    return RhdcDownloadDialog::runForRhdcUrl(romPath.u8string());
  }

  static const std::set<string> s_allowedExtensions = std::set<string>{
      ".z64", ".n64", ".v64", ".Z64", ".N64", ".V64", ".bps", ".BPS", ".Bps"};

  try {
    if (!fs::isRegularFileSafe(romPath)) {
      return fs::path();
    }

    if (s_allowedExtensions.count(romPath.extension().u8string()) <= 0) {
      return fs::path();
    }

    if (romPath.is_relative()) {
      return fs::current_path() / romPath;
    }

    return romPath;

  } catch (...) {
  }

  return fs::path();
}

static inline void
loadTranslations(QTranslator &translations,
                 std::initializer_list<fs::path> translationDirectories) {
  const QLocale locale = QLocale();
  for (const fs::path &folder : translationDirectories) {
    if (translations.load(locale, "parallel-launcher", "_",
                          folder.u8string().c_str())) {
      return;
    }
  }
}

static string makeQtLogMessage(const QMessageLogContext &context,
                               const QString &message) {
  return QString("[Qt] %1\nin %2 at %3:%4 (v%5)")
      .arg(message)
      .arg(context.file)
      .arg(context.function)
      .arg(context.line)
      .arg(context.version)
      .toStdString();
}

static void onQtMessage(QtMsgType level, const QMessageLogContext &context,
                        const QString &message) {
  switch (level) {
  case QtMsgType::QtWarningMsg:
    logWarn(makeQtLogMessage(context, message));
    break;
  case QtMsgType::QtCriticalMsg:
    logError(makeQtLogMessage(context, message));
    break;
  case QtMsgType::QtFatalMsg:
    logFatal(makeQtLogMessage(context, message));
    break;
  default:
    break;
  }
}

#ifdef FLATPAK_VERSION
static inline void fixDiscordIntegrationInFlatpak() {
  const char *xdgrd = std::getenv("XDG_RUNTIME_DIR");
  if (xdgrd == nullptr || xdgrd[0] == '\0')
    return;

  for (char i = '0'; i <= '9'; i++) {
    const fs::path socketPath = fs::path(xdgrd) / ("discord-ipc-"s + i);
    if (fs::is_socket(socketPath))
      continue;

    fs::error_code err;
    fs::remove(socketPath, err);
    fs::create_symlink(fs::path(xdgrd) / "app" / "com.discordapp.Discord" /
                           ("discord-ipc-"s + i),
                       socketPath, err);
  }
}
#endif

static inline void createEmulatorCoreInfoFiles() {
  fs::error_code err;
  const fs::path infoDir = BaseDir::data() / _NFS("retro-data") / _NFS("info");

  fs::remove_all(infoDir, err);
  fs::create_directories(infoDir, err);
  QFile(":/parallel_n64_next_libretro.info")
      .copy((infoDir / _NFS("parallel_n64_next_libretro.info"))
                .u8string()
                .c_str());
  QFile(":/mupen64plus_next_libretro.info")
      .copy((infoDir / _NFS("mupen64plus_next_libretro.info"))
                .u8string()
                .c_str());
}
int main(int argc, char **argv) {
  initCoreAppContext();
  ThreadSafety::registerMainThread();
  registerCrashHandlers();
  storeCrashLog();

  QApplication app(argc, argv);

#ifdef __linux__
  QGuiApplication::setDesktopFileName(
      "ca.parallel_launcher.ParallelLauncher.desktop");
#endif

#ifdef FLATPAK_VERSION
  fixDiscordIntegrationInFlatpak();
#endif
  applyUiFixes(app);
  qInstallMessageHandler(onQtMessage);

  AppSettings settings = FileController::loadAppSettings();

  if (settings.locale.empty()) {
#ifdef _WIN32
    QLocale::setDefault(QLocale(WindowsLocale::getUserLocale().c_str()));
#endif
  } else {
    QLocale::setDefault(QLocale(settings.locale.c_str()));
  }

  initExtraAppContext();

  QTranslator translations;
  loadTranslations(translations, {
#ifdef __linux__
#if defined(FLATPAK_VERSION)
    fs::path("/app/share/parallel-launcher/translations/")
#elif defined(DEBUG)
                                       BaseDir::program() / "lang",
                                       fs::path("/usr/local/share/parallel-launcher/translations/"),
                                       fs::path("/usr/share/parallel-launcher/translations/")
#else
                                       fs::path("/usr/share/parallel-launcher/translations/"),
                                       fs::path("/usr/local/share/parallel-launcher/translations/"),
                                       BaseDir::program() / "lang"
#endif
#elif __APPLE__
                                       BaseDir::program().parent_path() / "Resources" / "translations"
#else
                                       BaseDir::program() / L"translations"
#endif
  });
  app.installTranslator(&translations);

  if (QLocale().language() == QLocale::Arabic) {
    app.setLayoutDirection(Qt::RightToLeft);
  }

  createEmulatorCoreInfoFiles();

#ifndef FLATPAK_VERSION
  if (!RetroArch::isRetroArchInstalled()) {
    if (QMessageBox::information(
            nullptr,
            QCoreApplication::translate("Main", "Installing RetroArch"),
            QCoreApplication::translate(
                "Main", "Parallel Launcher will now install RetroArch."),
            QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok) {
      std::exit(0);
    }

    if (!RetroArchUpdater::install()) {
      QMessageBox::critical(
          nullptr, QCoreApplication::translate("Main", "Fatal Error"),
          QCoreApplication::translate("Main", "Failed to install RetroArch."));
      std::exit(1);
    }

#ifdef _WIN32
    RetroArch::fixSdlLibrary();
  } else if (!RetroArch::hasCorrectSdlLibrary()) {
    RetroArch::fixSdlLibrary();
#endif
  }
#endif

#ifdef _WIN32
  SdCardManager::recoverSdCards();
#endif

#ifndef __linux__
  SelfUpdater::checkForUpdates();
#endif

  fs::path romPath = (argc >= 2) ? tryGetRomPath(argc, argv) : fs::path();
  if (!romPath.empty()) {
    romPath = processRomPath(romPath);
  }

  return romPath.empty() ? runMainWindow(app) : DirectPlay::start(app, romPath);
}

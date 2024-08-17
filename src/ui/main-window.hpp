#ifndef SRC_UI_MAIN_WINDOW_HPP_
#define SRC_UI_MAIN_WINDOW_HPP_

#include "src/core/async.hpp"
#include "src/core/controller.hpp"
#include "src/core/rom.hpp"
#include "src/core/settings.hpp"
#include "src/ui/rom-settings-widget.hpp"
#include <QMainWindow>
#include <QWidget>
#include <memory>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

private:
  Ui::MainWindow *m_ui;
  bool m_showAllPlugins;
  bool m_windowSizingComplete;
  AppSettings m_settings;
  std::shared_ptr<bool> m_exists;
  CancellationToken m_romSearch;
  RomSettingsWidget *m_romSettingsWidget;

  void play(bool multiplayer);
  void setView(int view, bool keepSize = false);

public:
  void refetchAll();

private slots:
  void refreshRomList();

  void playSingleplayer();
  void playMultiplayer();
public:
  MainWindow();
  virtual ~MainWindow();
};

#endif /* SRC_UI_MAIN_WINDOW_HPP_ */

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
  void updateRhdcActions();
  RomInfo *getSelectedRomInfo();
  void setView(int view, bool keepSize = false);

public:
  void refetchAll();

private slots:
  void reloadSettings();
  void refreshRomList();

  void editSave(fs::path saveFilePath);
  void playSingleplayer();
  void playMultiplayer();
  void configureKeyboard();
  void manageRomSources();
  void addRom();
  void editSettings();

  void rhdcLogin();
  void rhdcLogout();
  void rhdcDisable();

  void openSaveFileDirectory() const;
  void openSavestateDirectory() const;
  void openSdCardDirectory() const;
  void openDataDirectory() const;
  void openConfigDirectory() const;
  void openCacheDirectory() const;

public:
  MainWindow();
  virtual ~MainWindow();

protected:
  virtual void paintEvent(QPaintEvent *event) override;
  virtual void closeEvent(QCloseEvent *event) override;
  virtual void keyPressEvent(QKeyEvent *event) override;
};

#endif /* SRC_UI_MAIN_WINDOW_HPP_ */

#ifndef SRC_UI_DIRECT_PLAY_DIALOG_HPP_
#define SRC_UI_DIRECT_PLAY_DIALOG_HPP_

#include "src/core/filesystem.hpp"
#include "src/core/rom.hpp"
#include <QMainWindow>

namespace Ui {
class DirectPlayWindow;
}

class DirectPlayWindow : public QMainWindow {
  Q_OBJECT

private:
  const RomFile m_romFile;
  Ui::DirectPlayWindow *m_ui;

public:
  DirectPlayWindow(const RomFile &romFile);
  ~DirectPlayWindow();

  RomInfo getRomInfo() const;

private slots:
  void accept();

signals:
  void play();
};

#endif /* SRC_UI_DIRECT_PLAY_DIALOG_HPP_ */

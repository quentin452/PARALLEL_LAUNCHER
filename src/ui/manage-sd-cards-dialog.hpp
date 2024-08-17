#ifndef SRC_UI_MANAGE_SD_CARDS_DIALOG_HPP_
#define SRC_UI_MANAGE_SD_CARDS_DIALOG_HPP_

#include "src/core/sdcard.hpp"
#include <QDialog>
#include <memory>
#include <unordered_map>

namespace Ui {
class ManageSdCardsDialog;
}

class ManageSdCardsDialog : public QDialog {
  Q_OBJECT

private:
  Ui::ManageSdCardsDialog *m_ui;
  std::vector<SdCardInfo> m_cards;
  HashMap<std::string, std::shared_ptr<MountedSdCard>> m_mountedCards;
  std::shared_ptr<bool> m_alive;
  bool m_creating;

public:
  ManageSdCardsDialog(QWidget *parent);
  ~ManageSdCardsDialog();

private:
  std::string generateName() const;
  void refreshList();

  static void createAsync(QString name, std::shared_ptr<AsyncProcess> task,
                          std::shared_ptr<bool> alive,
                          ManageSdCardsDialog *dialog);

  bool tryUnmountAll();
  bool shouldClose();

  bool unmountOne(const std::string &cardName);

protected:
  virtual void closeEvent(QCloseEvent *event);

protected slots:
  virtual void accept();
  virtual void reject();

private slots:
  void nameChanged(QString name);
  void formatChanged(int format);
  void deleteCard();
  void createCard();
  void discard();
  void beginCreate();
  void cardSelected(int cardIndex);
  void browseFiles();
  void cloneCard();
};

#endif /* SRC_UI_MANAGE_SD_CARDS_DIALOG_HPP_ */

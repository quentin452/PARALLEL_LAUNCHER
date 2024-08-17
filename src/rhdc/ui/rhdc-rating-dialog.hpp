#ifndef SRC_RHDC_UI_RHDC_RATING_DIALOG_HPP_
#define SRC_RHDC_UI_RHDC_RATING_DIALOG_HPP_

#include "src/rhdc/core/hack.hpp"
#include "src/types.hpp"
#include <QDialog>

namespace Ui {
class RhdcRatingDialog;
}

class RhdcRatingDialog : public QDialog {
  Q_OBJECT

private:
  Ui::RhdcRatingDialog *m_ui;
  RhdcRating m_rating;
  bool m_kaizo;

public:
  RhdcRatingDialog(QWidget *parent = nullptr);
  ~RhdcRatingDialog();

  void setRatings(RhdcRating quality, ubyte difficulty, bool isKaizo);
  RhdcRating getQuality() const;
  ubyte getDifficulty() const;

private:
  void updateButtons();

  void setQualityRating(RhdcRating rating) {
    if (m_rating == rating) {
      m_rating = RhdcRating::NotRated;
    } else {
      m_rating = rating;
    }
    updateButtons();
  }

private slots:
  void difficultyChanged(int rating);
  void dislikedClicked();
  void neutralClicked();
  void likedClicked();
  void lovedClicked();
};

#endif /* SRC_RHDC_UI_RHDC_RATING_DIALOG_HPP_ */

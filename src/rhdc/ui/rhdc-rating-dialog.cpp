#include "src/rhdc/ui/rhdc-rating-dialog.hpp"
#include "ui_rhdc-rating-dialog.h"

#include <QPushButton>
#include "src/core/numeric-string.hpp"
#include "src/ui/icons.hpp"
#include "src/ui/util.hpp"

static const char *s_difficultyLabels[11] = {
	/*:  0 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "I can't decide or have no opinion" ),
	/*:  1 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "No Challenge" ),
	/*:  2 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Very Easy" ),
	/*:  3 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Casual" ),
	/*:  4 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Classic SM64" ),
	/*:  5 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Moderate" ),
	/*:  6 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Challenging" ),
	/*:  7 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Difficult" ),
	/*:  8 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Very Difficult" ),
	/*:  9 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Extremely Difficult" ),
	/*: 10 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Borderline Kaizo" )
};

static const char *s_kaizoLabels[11] = {
	/*:  0 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "I can't decide or have no opinion" ),
	/*:  1 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Beginner/Introductory Kaizo" ),
	/*:  2 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Easy Kaizo" ),
	/*:  3 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Standard Kaizo" ),
	/*:  4 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Moderate Kaizo" ),
	/*:  5 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Challenging Kaizo" ),
	/*:  6 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Difficult Kaizo" ),
	/*:  7 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Very Difficult Kaizo" ),
	/*:  8 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Extremely Difficult Kaizo" ),
	/*:  9 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "Hardest humanly possible Kaizo" ),
	/*: 10 */ QT_TRANSLATE_NOOP( "RhdcRatingDialog", "TAS/Unbeatable" )
};

RhdcRatingDialog::RhdcRatingDialog( QWidget *parent ) :
	QDialog( parent ),
	m_ui( new Ui::RhdcRatingDialog ),
	m_kaizo( false )
{
	m_ui->setupUi( this );
	setWindowIcon( Icon::appIcon() );
	setModal( true );
	m_ui->buttonTray->button( QDialogButtonBox::Ok )->setText( tr( "Submit" ) );
	UiUtil::fixFontSizeOnMac( m_ui->label );
	UiUtil::fixDialogButtonsOnWindows( this );
	UiUtil::setIconSize( m_ui->dislikeButton, 32 );
	UiUtil::setIconSize( m_ui->neutralButton, 32 );
	UiUtil::setIconSize( m_ui->likeButton, 32 );
	UiUtil::setIconSize( m_ui->loveButton, 32 );
	updateButtons();
}

RhdcRatingDialog::~RhdcRatingDialog() {
	delete m_ui;
}

void RhdcRatingDialog::setRatings( RhdcRating quality, ubyte difficulty, bool isKaizo ) {
	m_kaizo = isKaizo;
	m_rating = quality;
	m_ui->difficultySlider->setValue( (int)difficulty );
	difficultyChanged( (int)difficulty );
	updateButtons();
}

RhdcRating RhdcRatingDialog::getQuality() const {
	return m_rating;
}

ubyte RhdcRatingDialog::getDifficulty() const {
	return (ubyte)m_ui->difficultySlider->value();
}

static QString makeScoreLabel( int score ) {
	if( score == 0 ) return RhdcRatingDialog::tr( "Not Rated" );
	return QString( (Number::toString( score ) + "/10").c_str() );
}

void RhdcRatingDialog::difficultyChanged( int rating ) {
	m_ui->difficultyScoreLabel->setText( makeScoreLabel( rating ) );
	m_ui->difficultyInfoLabel->setText( m_kaizo ? tr( s_kaizoLabels[rating] ) : tr( s_difficultyLabels[rating] ) );
}

void RhdcRatingDialog::updateButtons() {
	m_ui->dislikeButton->setChecked( m_rating == RhdcRating::Disliked );
	m_ui->dislikeButton->setIcon( (m_rating == RhdcRating::Disliked) ? Icon::ratingDislikedFilled() : Icon::ratingDisliked() );
	m_ui->neutralButton->setChecked( m_rating == RhdcRating::Neutral );
	m_ui->neutralButton->setIcon( (m_rating == RhdcRating::Neutral) ? Icon::ratingNeutralFilled() : Icon::ratingNeutral() );
	m_ui->likeButton->setChecked( m_rating == RhdcRating::Liked );
	m_ui->likeButton->setIcon( (m_rating == RhdcRating::Liked) ? Icon::ratingLikedFilled() : Icon::ratingLiked() );
	m_ui->loveButton->setChecked( m_rating == RhdcRating::Loved );
	m_ui->loveButton->setIcon( (m_rating == RhdcRating::Loved) ? Icon::ratingLovedFilled() : Icon::ratingLoved() );
}

void RhdcRatingDialog::dislikedClicked() {
	setQualityRating( RhdcRating::Disliked );
}

void RhdcRatingDialog::neutralClicked() {
	setQualityRating( RhdcRating::Neutral );
}

void RhdcRatingDialog::likedClicked() {
	setQualityRating( RhdcRating::Liked );
}

void RhdcRatingDialog::lovedClicked() {
	setQualityRating( RhdcRating::Loved );
}

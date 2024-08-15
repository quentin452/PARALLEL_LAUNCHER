#ifndef SRC_RHDC_UI_STAR_BIT_TOGGLE_HPP_
#define SRC_RHDC_UI_STAR_BIT_TOGGLE_HPP_

#include <QLabel>

class StarBitToggle : public QLabel {
	Q_OBJECT

	private:
	const QPixmap m_checkedImage;
	const QPixmap m_uncheckedImage;
	const bool m_editable;
	bool m_checked;

	public:
	StarBitToggle(
		QWidget *parent,
		const QPixmap &checkedImage,
		const QPixmap &uncheckedImage,
		bool editable,
		bool checked
	);

	~StarBitToggle() {}

	protected:
	virtual void mousePressEvent( QMouseEvent *event ) override;
	virtual void keyPressEvent( QKeyEvent *event ) override;

	virtual QSize minimumSizeHint() const override;
	virtual QSize sizeHint() const override;

	public:
	inline bool checked() const noexcept { return m_checked; }
	void setChecked( bool checked );

	public slots:
	void toggle();

	signals:
	void toggled( bool checked );

};



#endif /* SRC_RHDC_UI_STAR_BIT_TOGGLE_HPP_ */

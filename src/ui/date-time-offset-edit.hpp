#ifndef SRC_UI_DATE_TIME_OFFSET_EDIT_HPP_
#define SRC_UI_DATE_TIME_OFFSET_EDIT_HPP_

#include <QDateTimeEdit>
#include <QTimer>

class DateTimeOffsetEdit : public QDateTimeEdit {
	Q_OBJECT

	private:
	QTimer m_timer;
	int m_offset;
	bool m_ignoreUpdate;

	public:
	DateTimeOffsetEdit( QWidget *parent );
	virtual ~DateTimeOffsetEdit() {}

	inline int offset() const noexcept { return m_offset; }
	void setOffset( int seconds );

	protected:
	virtual void hideEvent( QHideEvent *event ) override;
	virtual void showEvent( QShowEvent *event ) override;

	private slots:
	void syncOffset( const QDateTime &targetTime );
	void syncDate();
	void doneEditing();

};

#endif /* SRC_UI_DATE_TIME_OFFSET_EDIT_HPP_ */

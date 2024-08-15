#ifndef SRC_UI_STAR_ICON_HPP_
#define SRC_UI_STAR_ICON_HPP_

#include <QLabel>
#include "src/types.hpp"

class StarIcon : public QLabel {
	Q_OBJECT

	private:
	const ubyte m_courseId;
	const ubyte m_starId;
	bool m_collected;

	public:
	StarIcon(
		QWidget *parent,
		ubyte courseId,
		ubyte starId,
		bool collected = false
	);
	inline virtual ~StarIcon() {}

	inline bool isCannon() const { return m_starId >= 7; }
	inline bool isCollected() const { return m_collected; }
	inline ubyte courseId() const { return m_courseId; }
	inline ubyte starId() const { return m_starId; }

	void setCollected( bool collected );

	protected:
	virtual void mousePressEvent( QMouseEvent *ev ) override;

};



#endif /* SRC_UI_STAR_ICON_HPP_ */

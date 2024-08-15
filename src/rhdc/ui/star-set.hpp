#ifndef SRC_RHDC_UI_STAR_SET_HPP_
#define SRC_RHDC_UI_STAR_SET_HPP_

#include <QHBoxLayout>
#include "src/core/buffer.hpp"
#include "src/types.hpp"

class StarSetUi : public QHBoxLayout {
	Q_OBJECT

	private:
	const uint m_byteIndex;
	const uint m_activeByte;
	const uint m_activeMask;
	const ubyte m_mask;

	public:
	StarSetUi(
		QWidget *parent,
		const QPixmap &checkedImage,
		const QPixmap &uncheckedImage,
		bool editable,
		uint byteIndex,
		uint activeBit,
		ubyte mask,
		ubyte value = 0
	);
	~StarSetUi();

	void load( const Buffer &saveData );
	void save( Buffer &saveData ) const;

	private:
	void setValue( ubyte byte );

};



#endif /* SRC_RHDC_UI_STAR_SET_HPP_ */

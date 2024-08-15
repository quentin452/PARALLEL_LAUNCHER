#include "src/rhdc/ui/star-set.hpp"

#include "src/rhdc/ui/star-bit-toggle.hpp"

StarSetUi::StarSetUi(
	QWidget *parent,
	const QPixmap &checkedImage,
	const QPixmap &uncheckedImage,
	bool editable,
	uint byteIndex,
	uint activeBit,
	ubyte mask,
	ubyte value
) :
	QHBoxLayout( parent ),
	m_byteIndex( byteIndex ),
	m_activeByte( activeBit / 8 ),
	m_activeMask( 1 << (7 - (activeBit % 8)) ),
	m_mask( mask )
{
	setSizeConstraint( QLayout::SetMinimumSize );
	setDirection( QBoxLayout::LeftToRight );
	setContentsMargins( 0, 0, 0, 0 );
	setMargin( 0 );
	setSpacing( 6 );

	while( mask != 0 ) {
		const ubyte testBit = mask - (mask & (mask - 1));
		addWidget( new StarBitToggle( nullptr, checkedImage, uncheckedImage, editable, (value & testBit) != 0 ) );
		mask -= testBit;
	}
}

StarSetUi::~StarSetUi() {
	while( QLayoutItem *star = takeAt( 0 ) ) {
		star->widget()->deleteLater();
	}
}

void StarSetUi::load( const Buffer &saveData ) {
	const bool isActive = (ubyte)saveData.data()[m_activeByte] & m_activeMask;
	setValue( isActive ? (ubyte)saveData.data()[m_byteIndex] : 0 );
}

void StarSetUi::save( Buffer &saveData ) const {
	ubyte byte = (ubyte)saveData.data()[m_byteIndex];
	ubyte mask = m_mask;
	int i = 0;
	while( mask != 0 ) {
		const ubyte testBit = mask - (mask & (mask - 1));
		if( static_cast<StarBitToggle*>( itemAt( i++ )->widget() )->checked() ) {
			byte |= testBit;
		} else {
			byte &= ~testBit;
		}
		mask -= testBit;
	}
	saveData.data()[m_byteIndex] = (char)byte;
}

void StarSetUi::setValue( ubyte byte ) {
	ubyte mask = m_mask;
	int i = 0;
	while( mask != 0 ) {
		const ubyte testBit = mask - (mask & (mask - 1));
		static_cast<StarBitToggle*>( itemAt( i++ )->widget() )->setChecked( byte & testBit );
		mask -= testBit;
	}
}

#ifndef SRC_UI_PALETTE_HPP_
#define SRC_UI_PALETTE_HPP_

#ifndef __linux__
#include <QPalette>
#endif

namespace ColourPalette {

#ifndef __linux__
	extern const QPalette &light();
	extern const QPalette &dark();
#endif

	extern bool systemIsDarkMode();

}


#endif /* SRC_UI_PALETTE_HPP_ */

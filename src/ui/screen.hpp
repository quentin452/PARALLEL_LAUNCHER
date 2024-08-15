#ifndef SRC_UI_SCREEN_HPP_
#define SRC_UI_SCREEN_HPP_

#include <QScreen>

namespace Screen {

	extern QScreen *getActive();

	inline void getResolution( int &width, int &height ) {
		const QScreen *screen = getActive();
		if( !screen ) return;
		const QSize resolution = screen->size();
		width = resolution.width();
		height = resolution.height();
	}

	inline void getDesktopSize( int &width, int &height ) {
		const QScreen *screen = getActive();
		if( !screen ) return;
		const QSize resolution = screen->availableSize();
		width = resolution.width();
		height = resolution.height();
	}

	inline float getRefreshRate() {
		const QScreen *screen = getActive();
		return screen ? (float)screen->refreshRate() : 0.f;
	}

}



#endif /* SRC_UI_SCREEN_HPP_ */

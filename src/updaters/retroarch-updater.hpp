#ifndef SRC_UPDATERS_RETROARCH_UPDATE_DIALOG_HPP_
#define SRC_UPDATERS_RETROARCH_UPDATE_DIALOG_HPP_

namespace RetroArchUpdater {

#ifdef FLATPAK_VERSION
	inline bool install() noexcept { return false; }
	inline void update() noexcept {}
#else
	extern bool install();
	extern void update();
#endif

}

#endif /* SRC_UPDATERS_RETROARCH_UPDATE_DIALOG_HPP_ */

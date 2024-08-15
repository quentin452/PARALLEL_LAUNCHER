#ifndef SRC_CORE_RETROARCH_SDL_FIX_HPP_
#define SRC_CORE_RETROARCH_SDL_FIX_HPP_

#ifdef _WIN32
namespace RetroArch {

	extern bool hasCorrectSdlLibrary();
	extern bool fixSdlLibrary();

}
#endif

#endif /* SRC_CORE_RETROARCH_SDL_FIX_HPP_ */

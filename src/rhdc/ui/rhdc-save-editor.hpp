#ifndef SRC_RHDC_UI_RHDC_SAVE_EDITOR_HPP_
#define SRC_RHDC_UI_RHDC_SAVE_EDITOR_HPP_

#include "src/core/filesystem.hpp"
#include "src/types.hpp"

namespace RhdcSaveEditor {

	extern void exec(
		const fs::path &saveFilePath,
		const string &hackId
	);

}



#endif /* SRC_RHDC_UI_RHDC_SAVE_EDITOR_HPP_ */

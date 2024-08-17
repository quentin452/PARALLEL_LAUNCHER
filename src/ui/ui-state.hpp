#ifndef SRC_UI_UI_STATE_HPP_
#define SRC_UI_UI_STATE_HPP_

#include "src/core/filesystem.hpp"
#include "src/core/json.hpp"
#include "src/rhdc/ui/rhdc-ui-state.hpp"
#include "src/types.hpp"
#include <vector>

struct TreeUiState {
  fs::path selectedRom;
  string selectedGroup;
  std::vector<string> expandedGroups;
};

struct WindowSize {
  int width;
  int height;
};

struct UiState {
  TreeUiState romList;
  RhdcUiState rhdcList;
  WindowSize classicViewSize;
  WindowSize rhdcViewSize;
  bool showAllPlugins;
  bool rhdcView;

  static const UiState Default;
};

namespace JsonSerializer {
template <> void serialize<UiState>(JsonWriter &jw, const UiState &obj);
template <> UiState parse<UiState>(const Json &json);
} // namespace JsonSerializer

#endif /* SRC_UI_UI_STATE_HPP_ */

#ifndef SRC_RHDC_CORE_LAYOUT_HPP_
#define SRC_RHDC_CORE_LAYOUT_HPP_

#include "src/core/buffer.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/json.hpp"
#include "src/core/sm64.hpp"
#include <istream>
#include <vector>

struct StarLayout {

  enum class SaveFormat : ubyte {
    EEPROM = 0,
    SRAM = 1,
    FlashRAM = 2,
    MemPak = 3,
    RawSRM = 4
  };

  enum class Side : ubyte { Top = 0, Right = 1, Bottom = 2, Left = 3 };

  struct StarData {
    uint offset;
    ubyte mask;
  };

  struct Course {
    string name;
    std::vector<StarData> stars;
  };

  struct Group {
    string name;
    Side side;
    std::vector<Course> courses;
  };

  ubyte numSlots;
  uint slotsStart;
  uint slotSize;
  uint activeBit;
  uint checksumOffset;
  SaveFormat saveFormat;
  std::vector<Group> groups;
  std::vector<ubyte> collectedStarIcon;
  std::vector<ubyte> missingStarIcon;

  Buffer getUsedSaveData(std::istream &saveFile) const;
  void writeSaveData(std::ostream &saveFile, Buffer &saveData) const;
  uint countStars(const fs::path &saveFilePath) const;

  static bool tryLoadLayout(const string &hackId, StarLayout &layout);
  static bool hasLayout(const string &hackId);
  static fs::path jsmlPath(const string &hackId);
  static fs::path layoutPath(const string &hackId);

  static StarLayout createDefault();
};

namespace JsonSerializer {
template <> StarLayout parse<StarLayout>(const Json &json);
}

#endif /* SRC_RHDC_CORE_LAYOUT_HPP_ */

#ifndef SRC_RHDC_CORE_HACK_HPP_
#define SRC_RHDC_CORE_HACK_HPP_

#include "src/core/flags.hpp"
#include "src/core/settings.hpp"
#include "src/types.hpp"

enum class RhdcHackFlag : uint_fast8_t {
  None = 0,
  NoOverclock = 0x01,
  DualAnalog = 0x02,
  BigEEPROM = 0x04,
  SupportsSD = 0x08
};

enum class RhdcRating : uint_fast8_t {
  NotRated = 0,
  Disliked = 1,
  Neutral = 2,
  Liked = 3,
  Loved = 4
};

DEFINE_FLAG_OPERATIONS(RhdcHackFlag, uint_fast8_t)

struct RhdcHackInfo {
  string hackId;
  string name;
  string slug;
  string description;
  int starCount;
  int downloads;
  double avgRating;
  double avgDifficulty;
  string category;
  bool submitStarpower;
};

struct BasicRhdcHackInfo {
  string hackId;
  string name;
  int starCount;
  bool isKaizo;
  bool submitStarpower;
};

struct RhdcHackProgress {
  RhdcRating rating;
  ubyte difficulty;
  ushort stars;
  bool completed;

  inline bool operator==(const RhdcHackProgress &other) const noexcept {
    return (rating == other.rating && difficulty == other.difficulty &&
            stars == other.stars && completed == other.completed);
  }

  inline bool operator!=(const RhdcHackProgress &other) const noexcept {
    return !(*this == other);
  }
};

struct RhdcHack {
  RhdcHackInfo info;
  RhdcHackProgress progress;
};

struct BasicRhdcHack {
  BasicRhdcHackInfo info;
  RhdcHackProgress progress;
};

struct StarpowerExt {
  RhdcHackProgress progress;
  int64 playTime;
  int starCount;
};

struct RhdcHackExt {
  RhdcHackInfo info;
  RhdcHackProgress progress;
  std::vector<string> authors;
  int64 lastPlayed;
  int64 playTime;
};

struct RhdcHackVersion {
  string sha1;
  string name;
  GfxPlugin recommendedPlugin;
  RhdcHackFlag hackFlags;
  bool archived;
};

struct RhdcSettingHints {
  GfxPlugin recommendedPlugin;
  RhdcHackFlag hackFlags;
};

#endif /* SRC_RHDC_CORE_HACK_HPP_ */

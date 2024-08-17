#include "src/rhdc/web/api-helpers.hpp"
#include "src/rhdc/web/api.hpp"
#include "src/rhdc/web/require-login.hpp"

#include "src/core/logging.hpp"

struct RatingUpdate {
  RhdcRating quality;
  ubyte difficulty;
};

struct StarsUpdate {
  ushort stars;
};

struct CompletionUpdate {
  bool isComplete;
};

struct PlayTimeUpdate {
  int64 playTime;
};

template <>
void JsonSerializer::serialize<RhdcRating>(JsonWriter &jw,
                                           const RhdcRating &quality) {
  switch (quality) {
  case RhdcRating::Disliked:
    jw.writeString("Disliked");
    break;
  case RhdcRating::Neutral:
    jw.writeString("Neutral");
    break;
  case RhdcRating::Liked:
    jw.writeString("Liked");
    break;
  case RhdcRating::Loved:
    jw.writeString("Loved");
    break;
  default:
    jw.writeNull();
    break;
  }
}

template <> RhdcRating JsonSerializer::parse(const Json &json) {
  if (!json.hasValue())
    return RhdcRating::NotRated;
  const string rating = json.getOrDefault<string>("");
  if (rating == "Disliked")
    return RhdcRating::Disliked;
  if (rating == "Neutral")
    return RhdcRating::Neutral;
  if (rating == "Liked")
    return RhdcRating::Liked;
  if (rating == "Loved")
    return RhdcRating::Loved;
  return RhdcRating::NotRated;
}

template <>
void JsonSerializer::serialize<RatingUpdate>(JsonWriter &jw,
                                             const RatingUpdate &obj) {
  jw.writeObjectStart();
  jw.writePropertyName("rating");
  serialize(jw, obj.quality);
  jw.writeProperty("difficulty", obj.difficulty);
  jw.writeObjectEnd();
}

template <>
void JsonSerializer::serialize<StarsUpdate>(JsonWriter &jw,
                                            const StarsUpdate &obj) {
  jw.writeObjectStart();
  jw.writeProperty("starPoints", obj.stars);
  jw.writeObjectEnd();
}

template <>
void JsonSerializer::serialize<CompletionUpdate>(JsonWriter &jw,
                                                 const CompletionUpdate &obj) {
  jw.writeObjectStart();
  jw.writeProperty("hackComplete", obj.isComplete);
  jw.writeObjectEnd();
}

template <>
void JsonSerializer::serialize<PlayTimeUpdate>(JsonWriter &jw,
                                               const PlayTimeUpdate &obj) {
  jw.writeObjectStart();
  jw.writeProperty("playTime", obj.playTime);
  jw.writeObjectEnd();
}

void RhdcApi::getStarpowerAsync(
    const string &hackId, const std::function<void(Starpower &)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN([=]() { getStarpowerAsync(hackId, onSuccess, onFailure); })
  QNetworkReply *response = ApiUtil::send(
      HttpMethod::Get, string(RHDC_API_HOST "/v3/starpower/user/") +
                           ApiUtil::identity().userId + "/hack/" + hackId);
  ApiUtil::onResponse(
      response,
      [=]() {
        const Json json = ApiUtil::readJson(response);
        if (json.isObject()) {
          Starpower starpower = {
              RhdcHackProgress{
                  JsonSerializer::parse<RhdcRating>(json["rating"]),
                  json["difficulty"].getOrDefault<ubyte>(0),
                  json["claimedStarPoints"].getOrDefault<ushort>(0),
                  json["claimedHackComplete"].getOrDefault<bool>(false)},
              json["playTime"].getOrDefault<int64>(0)};
          onSuccess(starpower);
        } else {
          logError("Unexpected JSON format in response from RHDC "
                   "gameusersubmission query");
          onFailure(ApiErrorType::JsonError);
        }
      },
      [=](ApiErrorType error) {
        if (error == ApiErrorType::NotFound) {
          Starpower starpower = {
              RhdcHackProgress{RhdcRating::NotRated, 0, 0, false}, 0};
          onSuccess(starpower);
        } else {
          onFailure(error);
        }
      });
}

void RhdcApi::submitRatingsAsync(
    const string &hackId, RhdcRating qualityRating, ubyte difficultyRating,
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN([=]() {
    submitRatingsAsync(hackId, qualityRating, difficultyRating, onSuccess,
                       onFailure);
  })
  QNetworkReply *response = ApiUtil::send<RatingUpdate>(
      HttpMethod::Patch, string(RHDC_API_HOST "/v3/starpower/hack/") + hackId,
      {qualityRating, difficultyRating});
  ApiUtil::onResponse(response, onSuccess, onFailure);
}

void RhdcApi::submitStarProgressAsync(
    const string &hackId, ushort starsCollected,
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN([=]() {
    submitStarProgressAsync(hackId, starsCollected, onSuccess, onFailure);
  })
  QNetworkReply *response = ApiUtil::send<StarsUpdate>(
      HttpMethod::Patch, string(RHDC_API_HOST "/v3/starpower/hack/") + hackId,
      {starsCollected});
  ApiUtil::onResponse(response, onSuccess, onFailure);
}

void RhdcApi::submitCompletionAsync(
    const string &hackId, bool isComplete,
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN([=]() {
    submitCompletionAsync(hackId, isComplete, onSuccess, onFailure);
  })
  QNetworkReply *response = ApiUtil::send<CompletionUpdate>(
      HttpMethod::Patch, string(RHDC_API_HOST "/v3/starpower/hack/") + hackId,
      {isComplete});
  ApiUtil::onResponse(response, onSuccess, onFailure);
}

void RhdcApi::submitPlayTimeAsync(
    const string &hackId, int64 playTime,
    const std::function<void(void)> &onSuccess,
    const std::function<void(ApiErrorType)> &onFailure) {
  REQUIRE_LOGIN(
      [=]() { submitPlayTimeAsync(hackId, playTime, onSuccess, onFailure); })
  QNetworkReply *response = ApiUtil::send<PlayTimeUpdate>(
      HttpMethod::Patch, string(RHDC_API_HOST "/v3/starpower/hack/") + hackId,
      {playTime});
  ApiUtil::onResponse(response, onSuccess, onFailure);
}

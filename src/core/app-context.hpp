#ifndef SRC_CORE_APP_CONTEXT_HPP_
#define SRC_CORE_APP_CONTEXT_HPP_

#include "src/types.hpp"

struct AppContextInfo {
  /* Core context info */
  const char *crashlogFilepath;
  const char *appVersion;
  int64 appStartTime;
  /* Extra context info */
  const char *deviceId;
  const char *locale;
  int screenWidth;
  int screenHeight;
  int screenDpiX;
  int screenDpiY;
};

extern const AppContextInfo &getAppContext();
extern void initCoreAppContext();
extern void initExtraAppContext();

#endif /* SRC_CORE_APP_CONTEXT_HPP_ */

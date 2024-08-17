#ifndef SRC_RHDC_CORE_CREDENTIALS_HPP_
#define SRC_RHDC_CORE_CREDENTIALS_HPP_

#include <string>

struct RhdcCredentials {
  std::string username;
  std::string jwt;
  std::string legacyPassword; // no longer used

  static RhdcCredentials load();
  static bool exists() noexcept;
  static void forget();
  void save() const;
};

#endif /* SRC_RHDC_CORE_CREDENTIALS_HPP_ */

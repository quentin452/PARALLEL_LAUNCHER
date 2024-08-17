#ifndef SRC_CORE_UTF8_HPP_
#define SRC_CORE_UTF8_HPP_

#include <string>

namespace Utf8 {

extern bool validate(const std::string &str, std::string *safeString = nullptr);

}

#endif /* SRC_CORE_UTF8_HPP_ */

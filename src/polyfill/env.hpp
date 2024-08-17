#ifndef SRC_POLYFILL_ENV_HPP_
#define SRC_POLYFILL_ENV_HPP_

namespace Environment {

bool set(const char *name, const char *value);
bool clear(const char *name);

} // namespace Environment

#endif /* SRC_POLYFILL_ENV_HPP_ */

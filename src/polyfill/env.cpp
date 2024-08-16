#include "src/polyfill/env.hpp"

#ifdef _WIN32

#include "src/polyfill/windows/unicode.hpp"
#include <windows.h>

bool Environment::set(const char *name, const char *value) {
    // Convert to wide string
    const std::wstring name16 = Unicode::toUtf16(name);
    const std::wstring value16 = Unicode::toUtf16(value);

    // Use the Unicode version of SetEnvironmentVariable
    return SetEnvironmentVariableW(name16.c_str(), value16.c_str()) != 0;
}

bool Environment::clear(const char *name) {
    // Convert to wide string
    const std::wstring name16 = Unicode::toUtf16(name);

    // Use the Unicode version of SetEnvironmentVariable
    return SetEnvironmentVariableW(name16.c_str(), nullptr) != 0;
}

#else

#include <stdlib.h>

bool Environment::set(const char *name, const char *value) {
    return setenv(name, value, 1) == 0;
}

bool Environment::clear(const char *name) {
    return unsetenv(name) == 0;
}

#endif
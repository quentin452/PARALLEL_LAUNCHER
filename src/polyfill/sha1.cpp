#include "src/polyfill/sha1.hpp"

#include "src/polyfill/file.hpp"

#include "src/polyfill/crash.hpp"

#include "src/types.hpp"

#ifdef _WIN32
#include <windows.h>
#include <Wincrypt.h>

static inline void computeHash(ubyte *data, ubyte *digest, size_t dataSize) {
    HCRYPTPROV winContext;
    HCRYPTHASH winDigest;

    if (!CryptAcquireContextW(&winContext, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        throw WinCryptException();
    }

    if (!CryptCreateHash(winContext, CALG_SHA1, 0, 0, &winDigest)) {
        CryptReleaseContext(winContext, 0);
        throw WinCryptException();
    }

    if (!CryptHashData(winDigest, data, (DWORD)dataSize, 0)) {
        CryptDestroyHash(winDigest);
        CryptReleaseContext(winContext, 0);
        throw WinCryptException();
    }

    DWORD digestSize = 20;
    if (!CryptGetHashParam(winDigest, HP_HASHVAL, digest, &digestSize, 0)) {
        CryptDestroyHash(winDigest);
        CryptReleaseContext(winContext, 0);
        throw WinCryptException();
    }

    CryptReleaseContext(winContext, 0);
    CryptDestroyHash(winDigest);
}
#else
#include <gcrypt.h>

static inline void computeHash(ubyte *data, ubyte *digest, size_t dataSize) {
    gcry_md_hash_buffer(GCRY_MD_SHA1, (void *)digest, (void *)data, dataSize);
}
#endif

inline char hexit(ubyte nibble) noexcept {
    if (nibble < 10) {
        return '0' + (char)nibble;
    } else {
        return 'a' + (char)(nibble - 10);
    }
}

string Sha1::compute(const fs::path &filePath) {
    if (!fs::isRegularFileSafe(filePath))
        return "";

    const std::uintmax_t fileSize = fs::file_size(filePath);
    if (fileSize == 0 || fileSize == static_cast<std::uintmax_t>(-1))
        return "";

    char *const data = new char[(size_t)fileSize];
    pl_assert(data != nullptr);

    InputFile file(filePath, true);
    file.read(data, (std::streamsize)fileSize);

    if (file.fail()) {
        delete[] data;
        return "";
    }

    ubyte digest[20];
    computeHash((ubyte *)data, digest, (size_t)fileSize);
    delete[] data;

    string result;
    result.reserve(40);
    for (int i = 0; i < 20; i++) {
        result += hexit(digest[i] >> 4);
        result += hexit(digest[i] & 0xF);
    }

    return result;
}

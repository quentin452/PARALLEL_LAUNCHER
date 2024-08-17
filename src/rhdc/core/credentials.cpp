#include "src/rhdc/core/credentials.hpp"

#include "src/core/filesystem.hpp"
#include "src/core/json.hpp"
#include "src/core/logging.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file.hpp"

static const ubyte s_key[72] = {
    0xcb, 0x9e, 0x6f, 0xdd, 0x22, 0x92, 0x56, 0x65, 0x25, 0x36, 0xa9, 0x64,
    0xf3, 0x26, 0x17, 0x8e, 0xbc, 0xcc, 0x71, 0x1a, 0xa7, 0x60, 0xe5, 0x2f,
    0x56, 0xaf, 0xd7, 0xc5, 0x5b, 0xea, 0xf1, 0xc2, 0x6c, 0xd3, 0x5a, 0x11,
    0xd3, 0xc4, 0xa0, 0xc7, 0xba, 0x11, 0x30, 0x8c, 0x47, 0x47, 0x80, 0xf7,
    0x2d, 0x28, 0x3e, 0x63, 0xac, 0x83, 0xd7, 0x5d, 0xda, 0xdd, 0xd3, 0xf3,
    0x92, 0xe8, 0xee, 0x47, 0x27, 0xc4, 0x5b, 0xf4, 0x34, 0x8b, 0x26, 0xdb};

/* As evinced by you reading the encryption key right now, this is not
 * intended to actually prevent a malicious actor who knows about
 * Parallel Launcher from decrypting your auth token. It is merely here
 * to guard against a potential malicious application that scans files
 * it has access to and looks for things that look like oauth2 tokens.
 */

static inline char toHex(ubyte nibble) {
  if (nibble < 10) {
    return (char)(nibble + '0');
  } else {
    return (char)(nibble - 10) + 'a';
  }
}

static inline ubyte fromHex(char c) {
  if (c >= '0' && c <= '9')
    return (ubyte)(c - '0');
  if (c >= 'a' && c <= 'f')
    return (ubyte)(c - 'a' + 10);
  if (c >= 'A' && c <= 'F')
    return (ubyte)(c - 'A' + 10);
  return 0;
}

static inline string encryptSecret(const string &plaintext) {
  string cyphertext;
  cyphertext.reserve(plaintext.length() * 2);
  size_t i = 0;
  for (char c : plaintext) {
    const ubyte x = (ubyte)c ^ s_key[i++ % 72];
    cyphertext += toHex(x >> 4);
    cyphertext += toHex(x & 0xF);
  }
  return cyphertext;
}

static inline string decryptSecret(const string &cyphertext) {
  if (cyphertext.empty())
    return ""s;

  string plaintext;
  plaintext.reserve(cyphertext.length() / 2);
  for (size_t i = 0; i < cyphertext.length() - 1; i += 2) {
    const ubyte c = (fromHex(cyphertext[i]) << 4) | fromHex(cyphertext[i + 1]);
    plaintext += (char)(c ^ s_key[(i / 2) % 72]);
  }
  return plaintext;
}

static inline void restrictFilePermissions(const fs::path &filePath) {
  fs::error_code err;
  fs::permissions(filePath, fs::perms::owner_read | fs::perms::owner_write,
#ifndef __APPLE__
                  fs::perm_options::replace,
#endif
                  err);
}

static constexpr char P_USERNAME[] = "username";
static constexpr char P_TOKEN[] = "token";

RhdcCredentials RhdcCredentials::load() {
  const fs::path filePath = BaseDir::data() / _NFS("rhdc-user.json");
  if (!fs::isRegularFileSafe(filePath)) {
    return RhdcCredentials{"", "", ""};
  }

  restrictFilePermissions(filePath);
  try {
    InputFile file(filePath, false);
    const Json json = Json::parse(file);
    return RhdcCredentials{
        json[P_USERNAME].getOrDefault<string>(""),
        decryptSecret(json[P_TOKEN].getOrDefault<string>("")),
        decryptSecret(json["password"].getOrDefault<string>(""))};
  } catch (const std::exception &ex) {
    logError("Failed to load RHDC credentials: "s + ex.what());
    return RhdcCredentials{"", "", ""};
  }
}

bool RhdcCredentials::exists() noexcept {
  return fs::existsSafe(BaseDir::data() / _NFS("rhdc-user.json"));
}

void RhdcCredentials::save() const {
  const fs::path filePath = BaseDir::data() / _NFS("rhdc-user.json");
  OutputFile file(filePath, false);
  JsonWriter jw(&file, true);
  jw.writeObjectStart();
  jw.writeProperty(P_USERNAME, username);
  jw.writeProperty(P_TOKEN, encryptSecret(jwt));
  jw.writeObjectEnd();
  file.flush();
  file.close();
  restrictFilePermissions(filePath);
}

void RhdcCredentials::forget() {
  fs::error_code err;
  fs::remove(BaseDir::data() / _NFS("rhdc-user.json"), err);
}

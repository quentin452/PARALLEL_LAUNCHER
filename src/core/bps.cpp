#include "src/core/bps.hpp"

#include "src/core/buffer.hpp"
#include "src/core/file-controller.hpp"
#include "src/core/logging.hpp"
#include "src/db/data-provider.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/file.hpp"
#include "src/thirdparty/libbps.h"
#include <cstring>
#include <memory>

class BpsFile : public file {

private:
  const size_t m_fileSize;
  std::unique_ptr<InputFile> m_file;

public:
  BpsFile(const fs::path &bpsPath)
      : m_fileSize((size_t)fs::file_size(bpsPath)) {
    m_file = std::make_unique<InputFile>(bpsPath, true);
  }

  ~BpsFile() {}

  size_t len() override { return m_fileSize; }

  bool read(uint8_t *target, size_t start, size_t len) override {
    m_file->seekg(start);
    m_file->read((char *)target, len);
    return m_file->good();
  }
};

static bool isBpsFile(const fs::path &bpsPath) {
  InputFile bpsFile(bpsPath, true);

  char fileHeader[4];
  bpsFile.read(fileHeader, 4);
  return bpsFile.good() && std::strncmp(fileHeader, "BPS1", 4) == 0;
}

static Bps::BpsApplyError applyBps(const fs::path &bpsPath,
                                   const fs::path &baseRomPath,
                                   BpsFile &bpsFile, const bpsinfo &bpsInfo,
                                   fs::path &patchedRomPath) {
  if (bpsInfo.error != bpserror::bps_ok) {
    logError("Error reading BPS patch info: "s + bpsPath.u8string());
    return Bps::BpsApplyError::InvalidBps;
  }

  const size_t baseRomSize = (size_t)fs::file_size(baseRomPath);
  Buffer patchData(bpsFile.len());
  if (!bpsFile.read(patchData.udata(), 0, bpsFile.len())) {
    logError("Error reading bps file: "s + bpsPath.u8string());
    return Bps::BpsApplyError::InvalidBps;
  }

  Buffer baseRomData(baseRomSize);
  {
    InputFile baseRom(baseRomPath, true);
    if (baseRom.bad()) {
      logError("Error opening base rom file: "s + baseRomPath.u8string());
      return Bps::BpsApplyError::NoBaseRom;
    }

    baseRom.read((char *)baseRomData.data(), baseRomSize);

    if (baseRom.bad()) {
      logError("Error reading the base rom file: "s + baseRomPath.u8string());
      return Bps::BpsApplyError::NoBaseRom;
    }
  }

  mem patchMem, baseRomMem, patchedRomMem;
  patchMem.ptr = patchData.udata();
  patchMem.len = bpsFile.len();
  baseRomMem.ptr = baseRomData.udata();
  baseRomMem.len = baseRomSize;

  bpserror result =
      bps_apply(patchMem, baseRomMem, &patchedRomMem, nullptr, false);
  CBuffer patchedRomData(patchedRomMem.ptr); // Take ownership of pointer

  if (result != bpserror::bps_ok) {
    logError("Error applying BPS patch (Error: "s +
             Number::toString((int)result) + "): " + bpsPath.u8string());
    return Bps::BpsApplyError::PatchFailed;
  }

  const AppSettings &settings = FileController::loadAppSettings();
  if (!patchedRomPath.empty()) {
    fs::error_code err;
    fs::create_directories(patchedRomPath.parent_path(), err);
  } else if (settings.patchToSameFolder) {
    patchedRomPath = bpsPath;
  } else {
    patchedRomPath =
        BaseDir::expandHome(settings.patchedRomFolder) / bpsPath.filename();

    fs::error_code err;
    fs::create_directories(patchedRomPath.parent_path(), err);
    if (err) {
      logError("Failed to create directory: "s +
               patchedRomPath.parent_path().u8string());
      return Bps::BpsApplyError::WriteError;
    }
  }
  patchedRomPath.replace_extension(".z64");

  OutputFile patchedRom(patchedRomPath, true);
  patchedRom.write(patchedRomData.data<char>(), patchedRomMem.len);

  if (!patchedRom.good()) {
    logError("Failed to write to file: "s + patchedRomPath.u8string());
    return Bps::BpsApplyError::WriteError;
  }

  return Bps::BpsApplyError::None;
}

Bps::BpsApplyError Bps::tryApplyBps(const fs::path &bpsPath,
                                    fs::path &patchedRomPath) {
  if (!isBpsFile(bpsPath)) {
    return BpsApplyError::InvalidBps;
  }

  BpsFile bpsFile(bpsPath);
  bpsinfo info = bps_get_info(&bpsFile, false);

  if (info.error != bpserror::bps_ok) {
    logError("Error reading BPS patch info: "s + bpsPath.u8string());
    return BpsApplyError::InvalidBps;
  }

  fs::path baseRomPath;
  if (!DataProvider::tryFetchRomPathWithCrc32(info.crc_in, baseRomPath)) {
    return BpsApplyError::NoBaseRom;
  }

  return applyBps(bpsPath, baseRomPath, bpsFile, info, patchedRomPath);
}

Bps::BpsApplyError Bps::tryApplyBps(const fs::path &bpsPath,
                                    const fs::path &baseRomPath,
                                    fs::path &patchedRomPath) {
  if (!isBpsFile(bpsPath)) {
    return BpsApplyError::InvalidBps;
  } else if (!fs::isRegularFileSafe(baseRomPath)) {
    return BpsApplyError::NoBaseRom;
  }

  BpsFile bpsFile(bpsPath);
  bpsinfo info = bps_get_info(&bpsFile, false);

  if (info.error != bpserror::bps_ok) {
    logError("Error reading BPS patch info: "s + bpsPath.u8string());
    return BpsApplyError::InvalidBps;
  }

  return applyBps(bpsPath, baseRomPath, bpsFile, info, patchedRomPath);
}

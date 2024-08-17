#include "src/core/save-converter.hpp"

#include "src/core/buffer.hpp"
#include "src/polyfill/file.hpp"
#include <cctype>
#include <cstring>

#ifdef _WIN32
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/crash.hpp"
#include "src/polyfill/windows/unicode.hpp"
#include <cwctype>
#include <exception>
#include <map>
#include <vector>
#endif

static std::wstring toLowerCase(const std::wstring &str) {
  std::wstring lower = str;
  for (wchar_t &c : lower) {
    c = (wchar_t)std::towlower((std::wint_t)c);
  }
  return lower;
}

static inline std::wstring getSaveName(const fs::path &savePath) {
  std::wstring saveName = savePath.stem().wstring();
  if (saveName.length() < 8 ||
      toLowerCase(savePath.extension().wstring()) != L".mpk") {
    return saveName;
  }

  const wchar_t lastChar = saveName.at(saveName.length() - 1);
  if (lastChar < L'1' || lastChar > L'4') {
    return saveName;
  }

  if (toLowerCase(saveName.substr(saveName.length() - 7, 6)) != L"_cont_") {
    return saveName;
  }

  return saveName.substr(0, saveName.length() - 7);
}

static Buffer readSaveFile(const fs::path &filePath, size_t maxSize,
                           bool &foundSaveData) {
  Buffer saveData(maxSize);
  if (!fs::existsSafe(filePath)) {
    std::memset(saveData.data(), 0, maxSize);
    return saveData;
  }

  InputFile saveFile(filePath, true);
  if (!saveFile.good()) {
    std::memset(saveData.data(), 0, maxSize);
    return saveData;
  }

  saveFile.seekg(0);
  saveFile.read(saveData.data(), maxSize);
  const std::streamsize bytesRead = saveFile.gcount();
  saveFile.close();

  if (bytesRead <= 0) {
    std::memset(saveData.data(), 0, maxSize);
    return saveData;
  } else if ((size_t)bytesRead < maxSize) {
    std::memset(&saveData.data()[bytesRead], 0, maxSize - (size_t)bytesRead);
  }

  foundSaveData |= true;
  return saveData;
}

bool SaveConverter::importFromProject64(const fs::path &destinationPath,
                                        const fs::path &sourcePath) {
  if (!fs::existsSafe(sourcePath)) {
    return false;
  }

  const fs::path sourceDir = sourcePath.parent_path();

  fs::error_code err;
  fs::create_directories(destinationPath.parent_path(), err);

  const std::wstring saveName = getSaveName(sourcePath);
  bool foundSaveData = false;

  const Buffer eeprom =
      readSaveFile(sourceDir / (saveName + L".eep"), 0x800u, foundSaveData);
  const Buffer mempak1 = readSaveFile(sourceDir / (saveName + L"_Cont_1.mpk"),
                                      0x8000u, foundSaveData);
  const Buffer mempak2 = readSaveFile(sourceDir / (saveName + L"_Cont_2.mpk"),
                                      0x8000u, foundSaveData);
  const Buffer mempak3 = readSaveFile(sourceDir / (saveName + L"_Cont_3.mpk"),
                                      0x8000u, foundSaveData);
  const Buffer mempak4 = readSaveFile(sourceDir / (saveName + L"_Cont_4.mpk"),
                                      0x8000u, foundSaveData);
  const Buffer sram =
      readSaveFile(sourceDir / (saveName + L".sra"), 0x8000u, foundSaveData);
  const Buffer flashram =
      readSaveFile(sourceDir / (saveName + L".fla"), 0x20000u, foundSaveData);

  if (!foundSaveData) {
    return false;
  }

  OutputFile srmFile(destinationPath, true);
  srmFile.seekp(0);
  srmFile.write(eeprom.data(), 0x800u);
  srmFile.write(mempak1.data(), 0x8000u);
  srmFile.write(mempak2.data(), 0x8000u);
  srmFile.write(mempak3.data(), 0x8000u);
  srmFile.write(mempak4.data(), 0x8000u);
  srmFile.write(sram.data(), 0x8000u);
  srmFile.write(flashram.data(), 0x20000u);
  srmFile.write(&eeprom.data()[0x200], 0x600u);
  srmFile.flush();

  return true;
}

#ifdef _WIN32
static void
getProject64SaveDirsHelper(std::map<std::wstring, fs::path> &matches,
                           const std::wstring &keySuffix,
                           const fs::path &baseDir) {
  if (!fs::isDirectorySafe(baseDir))
    return;

  for (auto const &i : fs::directory_iterator(baseDir)) {
    fs::error_code err;
    if (!i.is_directory(err) || err)
      continue;

    const std::wstring folderName = toLowerCase(i.path().filename().wstring());
    if (folderName.substr(0, 9) != L"project64")
      continue;
    if (!fs::isDirectorySafe(i.path() / L"Save"))
      continue;

    matches[folderName + keySuffix] = i.path() / L"Save";
  }
}

static inline std::vector<fs::path> getProject64SaveDirs() {
  std::map<std::wstring, fs::path> matches;
  const fs::path virtualStorePath =
      BaseDir::data().parent_path().parent_path() / L"VirtualStore";
  getProject64SaveDirsHelper(matches, L"/4",
                             fs::path(L"C:\\") / L"Program Files");
  getProject64SaveDirsHelper(matches, L"/3",
                             fs::path(L"C:\\") / L"Program Files (x86)");
  getProject64SaveDirsHelper(matches, L"/2",
                             virtualStorePath / L"Program Files");
  getProject64SaveDirsHelper(matches, L"/1",
                             virtualStorePath / L"Program Files (x86)");

  std::vector<fs::path> saveDirs;
  saveDirs.reserve(matches.size());
  for (auto i = matches.rbegin(); i != matches.rend(); i++) {
    saveDirs.push_back(std::move(i->second));
  }

  return saveDirs;
}

void SaveConverter::autoImportFromProject64(const fs::path &destinationPath,
                                            const string &internalRomName) {
  if (fs::existsSafe(destinationPath))
    return;

  static const std::vector<fs::path> sourceSaveDirs = getProject64SaveDirs();
  if (sourceSaveDirs.empty())
    return;

  std::wstring romName;
  try {
    romName = Unicode::toUtf16(internalRomName);
  } catch (const std::exception &) {
    logWarningReport(
        "Unicode Error",
        "Invalid UTF-8 string was not sanitized into valid UTF-8 correctly");
    return;
  }

  for (const fs::path &sourceSaveDir : sourceSaveDirs) {
    if (SaveConverter::importFromProject64(
            destinationPath, sourceSaveDir / (romName + L".eep")) ||
        SaveConverter::importFromProject64(
            destinationPath, sourceSaveDir / (romName + L"_Cont_1.mpk")) ||
        SaveConverter::importFromProject64(
            destinationPath, sourceSaveDir / (romName + L"_Cont_2.mpk")) ||
        SaveConverter::importFromProject64(
            destinationPath, sourceSaveDir / (romName + L"_Cont_3.mpk")) ||
        SaveConverter::importFromProject64(
            destinationPath, sourceSaveDir / (romName + L"_Cont_4.mpk")) ||
        SaveConverter::importFromProject64(
            destinationPath, sourceSaveDir / (romName + L".sra")) ||
        SaveConverter::importFromProject64(destinationPath,
                                           sourceSaveDir / (romName + L".fla")))
      return;
  }
}
#endif

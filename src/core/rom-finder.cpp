#include "src/core/rom-finder.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/filesystem.hpp"
#include "src/core/preset-controllers.hpp"
#include "src/core/qthread.hpp"
#include "src/core/time.hpp"
#include "src/db/data-provider.hpp"
#include "src/db/transaction.hpp"
#include "src/polyfill/file-search.hpp"
#include "src/polyfill/sha1.hpp"
#include "src/rhdc/core/rom-plugin-updater.hpp"
#include "src/rhdc/web/api.hpp"
#include <cstdlib>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef _WIN32
#include "src/core/retroarch.hpp"
#include "src/core/save-converter.hpp"
#endif

struct RomIdentifier {
  string internalName;
  uint crc32;
  N64InputTypes inputTypes;
};

struct RomFileWithGroups {
  RomFile romFile;
  std::set<string> groups;
};

static void
updateDatabase(std::shared_ptr<std::vector<RomFileWithGroups>> newRoms,
               std::shared_ptr<std::vector<RomFile>> changedRoms,
               std::shared_ptr<std::vector<fs::path>> lostRoms,
               std::shared_ptr<HashMap<string, RomIdentifier>> romData) {
  SqlTransaction transaction;

  for (const fs::path &path : *lostRoms) {
    DataProvider::deleteRomPath(path);
  }

  const AppSettings &settings = FileController::loadAppSettings();

  for (RomFileWithGroups &romAndGroups : *newRoms) {
    RomFile &rom = romAndGroups.romFile;

    DataProvider::addPendingPluginCheck(rom.sha1);
    DataProvider::addRomFile(rom);
    DataProvider::addRomInfo(RomInfo{// does nothing if the sha1 already exists
                                     rom.sha1,
                                     "",
                                     romData->at(rom.sha1).internalName,
                                     EmulatorCore::UseDefault,
                                     GfxPlugin::UseDefault,
                                     GfxPlugin::UseDefault,
                                     settings.parallelTexRectUpscaling,
                                     settings.parallelRemoveBorders,
                                     settings.glidenFramebufferEmulation,
                                     settings.glidenCorrectDepthCompare,
                                     false,
                                     0,
                                     0,
                                     std::move(romAndGroups.groups),
                                     true,
                                     false,
                                     romData->at(rom.sha1).crc32,
                                     DefaultInputModes::Normal.id,
                                     WidescreenMode::Off,
                                     false,
                                     false,
                                     romData->at(rom.sha1).inputTypes,
                                     ""});
  }

  for (const RomFile &rom : *changedRoms) {
    RomFile oldFile;
    if (DataProvider::tryFetchRomFile(rom.path, &oldFile) &&
        rom.sha1 != oldFile.sha1) {
      DataProvider::cloneRomInfo(
          oldFile.sha1, rom.sha1, romData->at(rom.sha1).internalName,
          romData->at(rom.sha1).crc32, romData->at(rom.sha1).inputTypes);

      if (DataProvider::countRomFilesWithSha1(oldFile.sha1) <= 1) {
        DataProvider::deleteRomInfo(oldFile.sha1);
      }
    }

    DataProvider::updateRomFile(rom);
  }

  transaction.commit();
}

static void
checkForPlugins(int64 timeoutAt,
                const std::function<void(RomFinder::SearchResults)> &callback) {
  const int64 timeLeft = timeoutAt - Time::nowMs();
  if (timeLeft <= 0u) {
    callback(RomFinder::SearchResults::Updated);
    return;
  }

  std::vector<string> checksums = DataProvider::getPendingPluginChecks();
  if (checksums.empty()) {
    callback(RomFinder::SearchResults::Updated);
    return;
  }

  const bool checkMore = (checksums.size() >= 1024);
  RhdcApi::getPluginSettings(
      checksums,
      [=](const std::vector<RhdcSettingHintsExt> &results) {
        for (const RhdcSettingHintsExt &options : results) {
          RomPluginUpdater::applyRhdcRecommendations(options);
        }

        DataProvider::removePendingPluginChecks();

        if (checkMore) {
          checkForPlugins(timeoutAt, callback);
        } else {
          callback(RomFinder::SearchResults::Updated);
        }
      },
      [=](ApiErrorType) { callback(RomFinder::SearchResults::Updated); },
      timeLeft);
}

static void recomputeRomData(
    std::shared_ptr<HashMap<fs::path, std::set<string>>> newRoms,
    std::shared_ptr<std::vector<fs::path>> changedRoms,
    std::shared_ptr<std::vector<fs::path>> lostRoms,
    const std::function<void(RomFinder::SearchResults)> &qtCallback,
    CancellationToken cancellationToken) {
  std::shared_ptr<HashMap<string, RomIdentifier>> romData(
      new HashMap<string, RomIdentifier>);
  std::shared_ptr<std::vector<RomFileWithGroups>> newRomFiles(
      new std::vector<RomFileWithGroups>);
  std::shared_ptr<std::vector<RomFile>> changedRomFiles(
      new std::vector<RomFile>);

  for (auto &[path, groups] : *newRoms) {
    if (cancellationToken.isCancelled()) {
      QtThread::safeAsync(
          [qtCallback]() { qtCallback(RomFinder::SearchResults::Cancelled); });
      return;
    }

    const string sha1 = Sha1::compute(path);
    newRomFiles->push_back(
        {RomFile{path, RomUtil::getLastModified(path), sha1, true},
         std::move(groups)});

    (*romData)[sha1] = {RomUtil::getInternalName(path), RomUtil::getCrc32(path),
                        RomUtil::readControllerTypes(path)};

#ifdef _WIN32
    const fs::path romPath = path;
    const string internalName = (*romData)[sha1].internalName;
    QtThread::safeAsync([romPath, internalName]() {
      SaveConverter::autoImportFromProject64(
          RetroArch::getSaveFilePath(romPath), internalName);
    });
#endif
  }

  for (const fs::path &path : *changedRoms) {
    if (cancellationToken.isCancelled()) {
      QtThread::safeAsync(
          [qtCallback]() { qtCallback(RomFinder::SearchResults::Cancelled); });
      return;
    }

    const string sha1 = Sha1::compute(path);
    changedRomFiles->push_back(
        {path, RomUtil::getLastModified(path), sha1, true});

    (*romData)[sha1] = {RomUtil::getInternalName(path), RomUtil::getCrc32(path),
                        RomUtil::readControllerTypes(path)};
  }

  if (cancellationToken.isCancelled())
    return;
  QtThread::safeAsync([=]() {
    if (cancellationToken.isCancelled()) {
      qtCallback(RomFinder::SearchResults::Cancelled);
    } else {
      updateDatabase(newRomFiles, changedRomFiles, lostRoms, romData);
      checkForPlugins(Time::nowMs() + 3000, qtCallback);
    }
  });
}

static void
analyzeChanges(std::shared_ptr<HashMap<fs::path, std::set<string>>> foundRoms,
               const std::function<void(RomFinder::SearchResults)> &qtCallback,
               CancellationToken cancellationToken) {
  std::shared_ptr<std::vector<fs::path>> changedRoms(new std::vector<fs::path>);
  std::shared_ptr<std::vector<fs::path>> lostRoms(new std::vector<fs::path>);

  for (const RomFile &knownFile : DataProvider::fetchLocalRomFiles()) {
    if (foundRoms->count(knownFile.path) > 0) {
      const int64 diff = std::abs(RomUtil::getLastModified(knownFile.path) -
                                  knownFile.lastModified);
      if (diff > 2) {
        changedRoms->push_back(knownFile.path);
      }
    } else {
      lostRoms->push_back(knownFile.path);
    }
    foundRoms->erase(knownFile.path);
  }

  if (foundRoms->empty() && changedRoms->empty()) {
    if (lostRoms->empty()) {
      checkForPlugins(Time::nowMs() + 3000, qtCallback);
      return;
    }

    SqlTransaction transaction;
    for (const fs::path &path : *lostRoms) {
      DataProvider::deleteRomPath(path);
    }
    transaction.commit();

    checkForPlugins(Time::nowMs() + 3000, qtCallback);
  } else {
    std::thread(recomputeRomData, foundRoms, changedRoms, lostRoms, qtCallback,
                cancellationToken)
        .detach();
  }
}

static void
scanRomSource(const RomSource &source,
              HashMap<fs::path, std::set<string>> &foundRoms,
              const std::function<void(RomFinder::SearchResults)> &qtCallback,
              CancellationToken &cancellationToken) {
  if (cancellationToken.isCancelled()) {
    QtThread::safeAsync(
        [qtCallback]() { qtCallback(RomFinder::SearchResults::Cancelled); });
    return;
  }

  const std::vector<fs::path> sourceRoms = FileSearch::find(
      source.folder,
#ifdef _WIN32
      "*.?64",
#else
      "*.[nvz]64",
#endif
      source.recursive ? source.maxDepth : (ubyte)1, source.followSymlinks,
      source.ignoreHidden, cancellationToken);

  for (fs::path romPath : sourceRoms) {
#ifdef _WIN32
    const char romType = romPath.extension().u8string().c_str()[1];
    if (romType != 'n' && romType != 'N' && romType != 'v' && romType != 'V' &&
        romType != 'z' && romType != 'Z')
      continue;
#endif

    if (source.followSymlinks) {
      HashSet<fs::path> visitedLinks;
      while (fs::isSymlinkSafe(romPath) && visitedLinks.count(romPath) == 0) {
        visitedLinks.insert(romPath);
        romPath = fs::read_symlink(romPath);
      }
    }

    if (fs::isRegularFileSafe(romPath)) {
      foundRoms[romPath].insert(source.autoTags.begin(), source.autoTags.end());
    }
  }
}

static void scan(std::function<void(RomFinder::SearchResults)> qtCallback,
                 fs::path rhdcDir, CancellationToken cancellationToken) {
  std::shared_ptr<HashMap<fs::path, std::set<string>>> foundRoms(
      new HashMap<fs::path, std::set<string>>);
  for (const RomSource &source : FileController::loadRomSources()) {
    scanRomSource(source, *foundRoms, qtCallback, cancellationToken);
  }

  if (fs::existsSafe(rhdcDir)) {
    const RomSource rhdcSource = {/* folder */ rhdcDir,
                                  /* recursive */ false,
                                  /* ignoreHidden */ false,
                                  /* followSymlinks */ false,
                                  /* maxDepth */ 1,
                                  /* autoTags */ std::set<string>()};

    scanRomSource(rhdcSource, *foundRoms, qtCallback, cancellationToken);
  }

  QtThread::safeAsync([=]() {
    if (cancellationToken.isCancelled()) {
      qtCallback(RomFinder::SearchResults::Cancelled);
    } else {
      for (const fs::path &romPath : DataProvider::fetchManualRomPaths()) {
        if (fs::existsSafe(romPath)) {
          (*foundRoms)[romPath];
        } else {
          DataProvider::deleteManualRomPath(romPath);
        }
      }

      analyzeChanges(foundRoms, qtCallback, cancellationToken);
    }
  });
}

CancellationToken
RomFinder::scanAsync(const std::function<void(SearchResults)> &qtCallback) {
  CancellationToken cancellationToken;
  std::thread(scan, qtCallback,
              FileController::loadRhdcSettings().downloadDirectory,
              cancellationToken)
      .detach();
  return cancellationToken;
}

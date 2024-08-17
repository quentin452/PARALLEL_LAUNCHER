#include "src/core/libpl.hpp"

#include "src/core/buffer.hpp"
#include "src/core/logging.hpp"
#include "src/core/numeric-string.hpp"
#include "src/core/sdcard.hpp"
#include "src/core/uuid.hpp"
#include "src/core/version.hpp"
#include "src/db/data-provider.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/byteswap.hpp"
#include "src/rhdc/core/libpl-rhdc.hpp"
#include "src/types.hpp"
#include <QCoreApplication>
#include <QMetaObject>
#include <functional>

static inline bool sdCardExists(const string &name) {
  if (name.empty() || name == "?")
    return false;
  SdCardInfo scratch;
  return SdCardManager::tryFetch(name, scratch);
}

static string getLibplSdCard(const string &uid) {
  string result;
  QMetaObject::invokeMethod(
      QCoreApplication::instance(),
      [&result, &uid]() { result = DataProvider::getLibplSdCard(uid); },
      Qt::BlockingQueuedConnection);
  return result;
}

static void updateSdCard(const string &sha1, const string &card) {
  QMetaObject::invokeMethod(
      QCoreApplication::instance(),
      [&sha1, &card]() { DataProvider::updateSdCard(sha1, card); },
      Qt::BlockingQueuedConnection);
}

static void registerLibplSdCard(const string &uid, const string &name) {
  QMetaObject::invokeMethod(
      QCoreApplication::instance(),
      [&uid, &name]() { DataProvider::registerLibplSdCard(uid, name); },
      Qt::BlockingQueuedConnection);
}

static void removeLibplSdCard(const string &uid) {
  QMetaObject::invokeMethod(
      QCoreApplication::instance(),
      [&uid]() { DataProvider::removeLibplSdCard(uid); },
      Qt::BlockingQueuedConnection);
}

static bool handleCoreLibplRequest(ubyte commandId, ushort payloadSize,
                                   const ubyte *payload,
                                   const WritableFifoPipe &pipe,
                                   RomInfo &romInfo, std::error_code &err) {
  switch (commandId) {
  case 0x00: { // libpl_get_launcher_version
    const ushort response[5] = {0x0000, htons(6),
                                htons(CurrentVersion::Application.major),
                                htons(CurrentVersion::Application.minor),
                                htons(CurrentVersion::Application.patch)};
    return pipe.write(10, &response, err);
  }
  case 0x01: { // libpl_get_sd_card_info
    if (payloadSize < 1 || payloadSize > 36) {
      const ubyte response[4] = {0x02, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    const string uid((const char *)payload, payloadSize);
    if (string(uid.c_str()).length() != uid.length()) {
      const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    const string name = getLibplSdCard(uid);
    if (name.empty()) {
      const ubyte response[4] = {0x00, 0x01, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    SdCardInfo cardInfo;
    if (!SdCardManager::tryFetch(name, cardInfo) ||
        cardInfo.numBlocks >= 0x3FFFFFu) {
      removeLibplSdCard(uid);
      const ubyte response[4] = {0x00, 0x04, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    const ushort responseSize = (ushort)cardInfo.name.length() + 6;
    const ubyte header[4] = {0x00, 0x00, (ubyte)(responseSize >> 8),
                             (ubyte)(responseSize & 0xff)};
    if (!pipe.write(4, header, err))
      return false;

    const uint cardBytes = htonl(cardInfo.numBlocks * 1024);
    if (!pipe.write(4, &cardBytes, err))
      return false;

    ubyte format = 0;
    switch (cardInfo.format) {
    case SdCardFormat::FAT12:
      format = 12;
      break;
    case SdCardFormat::FAT16:
      format = 16;
      break;
    case SdCardFormat::FAT32:
      format = 32;
      break;
    default:
      break;
    }

    if (!pipe.write(1, &format, err))
      return false;
    return pipe.write(cardInfo.name.length() + 1, cardInfo.name.c_str(), err);
  }
  case 0x02: { // libpl_create_sd_card
    if (payloadSize < 5 || payloadSize > 50) {
      const ubyte response[4] = {0x02, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    if (sdCardExists(romInfo.sdCard)) {
      const ubyte response[4] = {0x00, 0x01, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    SdCardInfo cardInfo;
    cardInfo.numBlocks = (uint)payload[0] * 1024;

    switch (payload[1]) {
    case 0: {
      if (cardInfo.numBlocks == 0) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::None;
      break;
    }
    case 12: {
      if (cardInfo.numBlocks > 32 * 1024) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::FAT12;
      break;
    }
    case 16: {
      if (cardInfo.numBlocks < 32 * 1024) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::FAT16;
      break;
    }
    case 32: {
      if (cardInfo.numBlocks < 32 * 1024) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::FAT32;
      break;
    }
    default: {
      const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }
    }

    cardInfo.name = string((const char *)&payload[2]);
    if (cardInfo.name.length() < 1 || cardInfo.name.length() > 11 ||
        cardInfo.name == "?") {
      const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    if (sdCardExists(cardInfo.name)) {
      const ubyte response[4] = {0x00, 0x06, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    const string uid((const char *)&payload[cardInfo.name.length() + 3],
                     (size_t)payloadSize - cardInfo.name.length() - 4);
    if (uid.length() < 1 || uid.length() > 36 ||
        string(uid.c_str()).length() != uid.length()) {
      const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    const string existingCard = getLibplSdCard(uid);
    if (!existingCard.empty()) {
      if (sdCardExists(existingCard)) {
        const ubyte response[4] = {0x00, 0x05, 0x00, 0x00};
        return pipe.write(4, response, err);
      } else {
        removeLibplSdCard(uid);
      }
    }

    if (SdCardManager::createCardAsync(cardInfo).join() != 0) {
      const ubyte response[4] = {0x00, 0x03, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    romInfo.sdCard = cardInfo.name;
    registerLibplSdCard(uid, cardInfo.name);
    updateSdCard(romInfo.sha1, cardInfo.name);

    const string sdCardPath =
        (BaseDir::data() / _NFS("sdcard") / fs::to_path(cardInfo.name + ".iso"))
            .u8string();
    const ubyte header[4] = {0x00, 0x00, (ubyte)(sdCardPath.length() >> 8),
                             (ubyte)(sdCardPath.length() & 0xff)};
    if (!pipe.write(4, header, err))
      return false;
    return pipe.write(sdCardPath.length(), sdCardPath.c_str(), err);
  }
  case 0x03: { // libpl_create_auto_sd_card
    if (payloadSize != 2) {
      const ubyte response[4] = {0x02, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    if (sdCardExists(romInfo.sdCard)) {
      const ubyte response[4] = {0x00, 0x01, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    SdCardInfo cardInfo;
    cardInfo.numBlocks = (uint)payload[0] * 1024;

    switch (payload[1]) {
    case 0: {
      if (cardInfo.numBlocks == 0) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::None;
      break;
    }
    case 12: {
      if (cardInfo.numBlocks < 1024 || cardInfo.numBlocks > 32 * 1024) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::FAT12;
      break;
    }
    case 16: {
      if (cardInfo.numBlocks < 32 * 1024) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::FAT16;
      break;
    }
    case 32: {
      if (cardInfo.numBlocks < 32 * 1024) {
        const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
        return pipe.write(4, response, err);
      }

      cardInfo.format = SdCardFormat::FAT32;
      break;
    }
    default: {
      const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }
    }

    uint i = 0;
    do {
      cardInfo.name = "AUTO" + Number::toString(i++);
    } while (sdCardExists(cardInfo.name));

    if (SdCardManager::createCardAsync(cardInfo).join() != 0) {
      const ubyte response[4] = {0x00, 0x03, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    romInfo.sdCard = cardInfo.name;
    updateSdCard(romInfo.sha1, cardInfo.name);

    const string sdCardPath =
        (BaseDir::data() / _NFS("sdcard") / fs::to_path(cardInfo.name + ".iso"))
            .u8string();
    const ubyte header[4] = {0x00, 0x00, (ubyte)(sdCardPath.length() >> 8),
                             (ubyte)(sdCardPath.length() & 0xff)};
    if (!pipe.write(4, header, err))
      return false;
    return pipe.write(sdCardPath.length(), sdCardPath.c_str(), err);
  }
  case 0x04: { // libpl_load_sd_card
    if (payloadSize < 1 || payloadSize > 36) {
      const ubyte response[4] = {0x02, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    if (sdCardExists(romInfo.sdCard)) {
      const ubyte response[4] = {0x00, 0x01, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    const string uid((const char *)payload, payloadSize);
    if (string(uid.c_str()).length() != uid.length()) {
      const ubyte response[4] = {0x06, 0x00, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    const string name = getLibplSdCard(uid);
    if (name.empty()) {
      const ubyte response[4] = {0x00, 0x04, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    SdCardInfo cardInfo;
    if (!SdCardManager::tryFetch(name, cardInfo) ||
        cardInfo.numBlocks >= 0x3FFFFFu) {
      removeLibplSdCard(uid);
      const ubyte response[4] = {0x00, 0x04, 0x00, 0x00};
      return pipe.write(4, response, err);
    }

    romInfo.sdCard = cardInfo.name;
    updateSdCard(romInfo.sha1, cardInfo.name);

    const string sdCardPath =
        (BaseDir::data() / _NFS("sdcard") / fs::to_path(cardInfo.name + ".iso"))
            .u8string();
    const ubyte header[4] = {0x00, 0x00, (ubyte)(sdCardPath.length() >> 8),
                             (ubyte)(sdCardPath.length() & 0xff)};
    if (!pipe.write(4, header, err))
      return false;
    return pipe.write(sdCardPath.length(), sdCardPath.c_str(), err);
  }
  default: {
    const ubyte response[4] = {0x01, 0x00, 0x00, 0x00};
    return pipe.write(4, response, err);
  }
  }
}

int LibplHandler::initialize() {
  const std::string randomId = Uuid::Random().toString();

  std::error_code err;
  if (!m_input.open("lplin-"s + randomId, err)) {
    logError("Failed to create libpl pipe for reading: "s + err.message());
    return err.value();
  }

  if (!m_output.open("lplout-"s + randomId, err)) {
    logError("Failed to create libpl pipe for writing: "s + err.message());
    return err.value();
  }

  return 0;
}

void LibplHandler::connect(const RomInfo &romInfo) {
}

void LibplHandler::disconnect() {
  m_output.cancel();
  m_input.cancel();
  if (m_thread.joinable()) {
    m_thread.join();
    clearRhdcLibplCache();
  }
}

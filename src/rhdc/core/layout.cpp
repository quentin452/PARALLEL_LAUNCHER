#include "src/rhdc/core/layout.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/logging.hpp"
#include "src/polyfill/base-directory.hpp"
#include "src/polyfill/byteswap.hpp"
#include "src/polyfill/file.hpp"
#include <QByteArray>

static constexpr char BASIC_SCHEMA[] =
    "https://parallel-launcher.ca/layout/basic-01/schema.json";
static constexpr char ADVANCED_SCHEMA[] =
    "https://parallel-launcher.ca/layout/advanced-01/schema.json";

static std::vector<ubyte> parseBase64(const string &base64) {
    if (base64.empty())
        return std::vector<ubyte>();
    const QByteArray stringData =
        QByteArray::fromRawData(base64.c_str(), (int)base64.size());
    const QByteArray bytes = QByteArray::fromBase64(stringData);
    return std::vector<ubyte>((const ubyte *)bytes.begin(),
                              (const ubyte *)bytes.end());
}

template <>
StarLayout JsonSerializer::parse<StarLayout>(const Json &json) {
    const string schema = json["$schema"].get<string>();
    const bool isAdvanced = (schema == ADVANCED_SCHEMA);

    if (!isAdvanced && schema != BASIC_SCHEMA) {
        throw JsonReaderException("Invalid schema");
    }

    StarLayout layout;
    if (isAdvanced) {
        const Json &format = json["format"];

        layout.numSlots = format["num_slots"].get<ubyte>();
        layout.slotsStart = format["slots_start"].get<uint>();
        layout.slotSize = format["slot_size"].get<uint>();
        layout.activeBit = format["active_bit"].get<uint>();

        const Json coToken = format["checksum_offset"];
        if (!coToken.exists()) {
            layout.checksumOffset = 54;
        } else if (coToken.isNull()) {
            layout.checksumOffset = 0;
        } else {
            layout.checksumOffset = coToken.get<uint>();
        }

        if (layout.checksumOffset != 0 &&
            (int)layout.checksumOffset > (int)layout.slotSize - 2) {
            if (coToken.exists()) {
                throw JsonReaderException("Invalid checksum_offset: checksum does not "
                                          "lie within the save slot.");
            }
            layout.checksumOffset = 0;
        }

        const uint maxSlotSize =
            (layout.checksumOffset > 0) ? layout.checksumOffset : layout.slotSize;
        if (layout.activeBit >= 8 * maxSlotSize) {
            throw JsonReaderException(
                "Invalid active_bit: bit index is outside of the save slot.");
        }

        const uint saveFileSize = (uint)layout.numSlots * layout.slotSize;
        if (saveFileSize > 0x48800u) {
            throw JsonReaderException("Invalid save file format (save file cannot be "
                                      "larger than 296960 bytes)");
        }

        const string saveType = format["save_type"].getOrDefault<string>("EEPROM");
        if (saveType == "MemPak") {
            layout.saveFormat = StarLayout::SaveFormat::MemPak;
            if (saveFileSize > 0x20000u) {
                throw JsonReaderException("The save data is too large to fit in MemPak "
                                          "save data (max size 131072 bytes)");
            } else if (saveFileSize > 0x8000u) {
                if (layout.slotsStart != 0) {
                    throw JsonReaderException(
                        "When using the MemPak save type with a total save file size "
                        "larger than 32kB, slots_start must be 0.");
                }
                if (layout.slotSize != 0x8000u) {
                    throw JsonReaderException(
                        "When using the MemPak save type with a total save file size "
                        "larger than 32kB, slot_size must be 32768.");
                }
            }
        } else if (saveType == "SRAM") {
            layout.saveFormat = StarLayout::SaveFormat::SRAM;
            if (saveFileSize > 0x8000u) {
                throw JsonReaderException(
                    "The save data is too large to fit in SRAM (max size 32768 bytes)");
            }
        } else if (saveType == "FlashRAM") {
            layout.saveFormat = StarLayout::SaveFormat::FlashRAM;
            if (saveFileSize > 0x20000u) {
                throw JsonReaderException("The save data is too large to fit in "
                                          "FlashRAM (max size 131072 bytes)");
            }
        } else if (saveType == "EEPROM") {
            layout.saveFormat = StarLayout::SaveFormat::EEPROM;
            if (saveFileSize > 0x800u) {
                throw JsonReaderException("The save data is too large to fit in EEPROM "
                                          "(max size 2048 bytes)");
            }
        } else if (saveType == "Multi") {
            layout.saveFormat = StarLayout::SaveFormat::RawSRM;
        } else
            throw JsonReaderException(
                "Invalid save_type (must be \"EEPROM\", \"SRAM\", \"FlashRAM\", "
                "\"MemPak\", or \"Multi\")");

        if (layout.numSlots == 0) {
            throw JsonReaderException("Invalid num_slots (cannot be 0)");
        }

        if (layout.slotSize == 0) {
            throw JsonReaderException("Invalid slot_size (cannot be 0)");
        }
    } else {
        layout.numSlots = json["skipFileSelect"].get<bool>() ? 1 : 4;
        layout.slotsStart = 0;
        layout.slotSize = 112;
        layout.activeBit = 95;
        layout.checksumOffset = 54;
        layout.saveFormat = StarLayout::SaveFormat::EEPROM;
    }

    const JArray &groupsJson = json["groups"].array();
    layout.groups.reserve(groupsJson.size());
    for (const Json &groupJson : groupsJson) {
        const string sideString = groupJson["side"].get<string>();

        StarLayout::Side side;
        if (sideString == "left") {
            side = StarLayout::Side::Left;
        } else if (sideString == "right") {
            side = StarLayout::Side::Right;
        } else if (sideString == "top") {
            side = StarLayout::Side::Top;
        } else if (sideString == "bottom") {
            side = StarLayout::Side::Bottom;
        } else {
            throw JsonReaderException(
                "Invalid side (must be \"left\", \"right\", \"top\", or \"bottom\")");
        }

        layout.groups.push_back({groupJson["name"].get<string>(), side,
                                 std::vector<StarLayout::Course>()});

        std::vector<StarLayout::Course> &courses =
            layout.groups[layout.groups.size() - 1].courses;
        const JArray &coursesJson = groupJson["courses"].array();
        courses.reserve(coursesJson.size());
        for (const Json &courseJson : coursesJson) {
            string name = courseJson["name"].getOrDefault<string>("");
            if (isAdvanced) {
                const JArray &starDataJson = courseJson["data"].array();
                std::vector<StarLayout::StarData> starData;
                starData.reserve(starDataJson.size());
                for (const Json &starDatum : starDataJson) {
                    const ubyte mask = starDatum["mask"].get<ubyte>();
                    if (mask == 0) {
                        throw JsonReaderException("Invalid mask (cannot be 0)");
                    }

                    const uint offset = starDatum["offset"].get<uint>();
                    if (offset >= layout.slotSize) {
                        throw JsonReaderException("Star data offset is too large and lies "
                                                  "outside of the save slot");
                    } else if (layout.checksumOffset != 0 &&
                               offset >= layout.checksumOffset) {
                        throw JsonReaderException("Star data offset is too large and lies "
                                                  "past the checksum location");
                    }

                    starData.push_back({offset, mask});
                }

                courses.push_back({std::move(name), std::move(starData)});
            } else {
                const ubyte courseId = courseJson["courseId"].get<ubyte>();
                const ubyte mask = courseJson["starMask"].get<ubyte>();

                if (courseId > 25) {
                    throw JsonReaderException(
                        "Invalid courseId (must be between 0 and 25)");
                }

                if (mask == 0) {
                    throw JsonReaderException("Invalid starMask (cannot be 0)");
                }

                courses.push_back({std::move(name),
                                   {{courseId > 0 ? ((uint)courseId + 11) : 8, mask}}});
            }
        }
    }

    layout.collectedStarIcon =
        parseBase64(json["collectedStarIcon"].getOrDefault<string>(""));
    layout.missingStarIcon =
        parseBase64(json["missingStarIcon"].getOrDefault<string>(""));

    return layout;
}

static void parseJsmlDescriptions(std::vector<StarLayout::Group> &groups,
                                  const JArray &descriptions,
                                  StarLayout::Side side, ubyte globalMask) {
    StarLayout::Group *currentGroup = nullptr;
    for (const Json &json : descriptions) {
        switch (json["Type"].getOrDefault<int>(3)) {
        case 0: {
            const int mask = json["starMask"].getOrDefault<int>(-1);
            const int offset = json["offset"].getOrDefault<int>(-1);

            if (mask <= 0 && offset <= 0) {
                groups.push_back({json["text"].getOrDefault<string>(""), side,
                                  std::vector<StarLayout::Course>()});
                currentGroup = &groups[groups.size() - 1];
            } else if (mask > 0 && mask <= 255 && offset > 0) {
                if (currentGroup == nullptr) {
                    groups.push_back({"", side, std::vector<StarLayout::Course>()});
                    currentGroup = &groups[groups.size() - 1];
                }

                currentGroup->courses.push_back(
                    {json["text"].getOrDefault<string>(""),
                     {{(uint)offset + 8,
                       (ubyte)(globalMask & (((uint)mask >> 1) | (1 << 7)))}}});
            }
            break;
        }
        case 1: {
            if (currentGroup == nullptr) {
                groups.push_back({"", side, std::vector<StarLayout::Course>()});
                currentGroup = &groups[groups.size() - 1];
            }

            const int mask = json["starMask"].getOrDefault<int>(-1);
            const int offset = json["offset"].getOrDefault<int>(-1);
            if (mask <= 0 || mask > 255 || offset < 0)
                continue;

            currentGroup->courses.push_back(
                {json["text"].getOrDefault<string>(""),
                 {{(uint)offset, (ubyte)(globalMask & (uint)mask)}}});
            break;
        }
        case 2: {
            groups.push_back({json["text"].getOrDefault<string>(""), side,
                              std::vector<StarLayout::Course>()});
            currentGroup = &groups[groups.size() - 1];
            break;
        }
        default:
            break;
        }
    }
}

static inline StarLayout migrateLegacyLayout(const fs::path &sourceFilePath,
                                             const fs::path &outputFilePath) {
    InputFile inFile(sourceFilePath, false);

    const Json json = Json::parse(inFile);
    std::vector<StarLayout::Group> groups;
    inFile.close();

    const ubyte globalMask =
        (1 << json["starsShown"].getOrDefault<ushort>(8)) - 1;

    if (json["courseDescription"].isArray()) {
        parseJsmlDescriptions(groups, json["courseDescription"].array(),
                              StarLayout::Side::Left, globalMask);
    }

    if (json["secretDescription"].isArray()) {
        parseJsmlDescriptions(groups, json["secretDescription"].array(),
                              StarLayout::Side::Right, globalMask);
    }

    const StarLayout layout = {
        4,
        0,
        112,
        95,
        54,
        StarLayout::SaveFormat::EEPROM,
        std::move(groups),
        parseBase64(json["goldStar"].getOrDefault<string>("")),
        parseBase64(json["darkStar"].getOrDefault<string>(""))};

    fs::error_code err;
    fs::create_directories(outputFilePath.parent_path(), err);

    OutputFile outFile(outputFilePath, false);
    JsonWriter writer(&outFile, false);

    writer.writeObjectStart();
    writer.writeProperty("$schema", (string)ADVANCED_SCHEMA);
    writer.writePropertyName("format");
    writer.writeObjectStart();
    writer.writeProperty("save_type", "EEPROM"s);
    writer.writeProperty("num_slots", layout.numSlots);
    writer.writeProperty("slots_start", layout.slotsStart);
    writer.writeProperty("slot_size", layout.slotSize);
    writer.writeProperty("active_bit", layout.activeBit);
    writer.writeProperty("checksum_offset", 54u);
    writer.writeObjectEnd();
    writer.writePropertyName("groups");
    writer.writeArrayStart();
    for (const StarLayout::Group &group : layout.groups) {
        writer.writeObjectStart();
        writer.writeProperty("name", group.name);
        writer.writeProperty(
            "side", group.side == StarLayout::Side::Left ? "left"s : "right"s);
        writer.writePropertyName("courses");
        writer.writeArrayStart();
        for (const StarLayout::Course &course : group.courses) {
            writer.writeObjectStart();
            writer.writeProperty("name", course.name);
            writer.writePropertyName("data");
            writer.writeArrayStart();
            for (const StarLayout::StarData &star : course.stars) {
                writer.writeObjectStart();
                writer.writeProperty("offset", star.offset);
                writer.writeProperty("mask", star.mask);
                writer.writeObjectEnd();
            }
            writer.writeArrayEnd();
            writer.writeObjectEnd();
        }
        writer.writeArrayEnd();
        writer.writeObjectEnd();
    }
    writer.writeArrayEnd();
    writer.writeProperty("collectedStarIcon",
                         json["goldStar"].getOrDefault<string>(""));
    writer.writeProperty("missingStarIcon",
                         json["darkStar"].getOrDefault<string>(""));
    writer.writeObjectEnd();

    outFile.flush();
    return layout;
}

fs::path StarLayout::jsmlPath(const string &hackId) {
    return BaseDir::data() / _NFS("jsml") / fs::to_path(hackId + ".json");
}

fs::path StarLayout::layoutPath(const string &hackId) {
    return BaseDir::data() / _NFS("layouts") / fs::to_path(hackId + ".json");
}

bool StarLayout::hasLayout(const string &hackId) {
    return fs::existsSafe(layoutPath(hackId)) || fs::existsSafe(jsmlPath(hackId));
}

bool StarLayout::tryLoadLayout(const string &hackId, StarLayout &layout) {
    try {
        if (fs::existsSafe(layoutPath(hackId))) {
            InputFile layoutFile(layoutPath(hackId), false);
            layout = JsonSerializer::parse<StarLayout>(Json::parse(layoutFile));
            return true;
        } else if (fs::existsSafe(jsmlPath(hackId))) {
            layout = migrateLegacyLayout(jsmlPath(hackId), layoutPath(hackId));
            return true;
        }
    } catch (const std::exception &ex) {
        logError("Failed to read star layout for hackId "s + hackId + ": \n" +
                 ex.what());
    } catch (...) {
        logError("Failed to read star layout for hackId "s + hackId);
    }

    return false;
}

/* Mupen and ParallelN64 current have a bug where the SRAM and FlashRAM
 * is incorrectly stored according to the endianness of the system.
 */
static void fixEndianness(uint *data, size_t words) {
#if defined(__APPLE__)
    if (!FileController::loadAppSettings().forceX86) {
        return;
    }
#endif

    for (size_t i = 0; i < words; i++) {
        data[i] = htonl(data[i]);
    }
}

Buffer StarLayout::getUsedSaveData(std::istream &saveFile) const {
    switch (saveFormat) {
    case SaveFormat::EEPROM: {
        Buffer saveData(0x800);
        saveFile.seekg(0);
        saveFile.read(saveData.data(), 0x800);
        return saveData;
    }
    case SaveFormat::SRAM: {
        Buffer saveData(0x8000);
        saveFile.seekg(0x20800u);
        saveFile.read(saveData.data(), 0x8000);
        fixEndianness((uint *)saveData.data(), 0x2000);
        return saveData;
    }
    case SaveFormat::FlashRAM: {
        Buffer saveData(0x20000);
        saveFile.seekg(0x28800);
        saveFile.read(saveData.data(), 0x20000);
        fixEndianness((uint *)saveData.data(), 0x8000);
        return saveData;
    }
    case SaveFormat::MemPak: {
        Buffer saveData(0x20000);
        saveFile.seekg(0x800);
        saveFile.read(saveData.data(), 0x20000);
        return saveData;
    }
    default: {
        Buffer saveData(0x48800);
        saveFile.seekg(0);
        saveFile.read(saveData.data(), 0x48800);
        fixEndianness((uint *)&saveData.data()[0x20800], 0xA000);
        return saveData;
    }
    }
}

void StarLayout::writeSaveData(std::ostream &saveFile, Buffer &saveData) const {
    switch (saveFormat) {
    case SaveFormat::EEPROM: {
        saveFile.seekp(0);
        saveFile.write(saveData.data(), 0x800);
        return;
    }
    case SaveFormat::SRAM: {
        fixEndianness((uint *)saveData.data(), 0x2000);
        saveFile.seekp(0x20800u);
        saveFile.write(saveData.data(), 0x8000);
        fixEndianness((uint *)saveData.data(), 0x2000);
        return;
    }
    case SaveFormat::FlashRAM: {
        fixEndianness((uint *)saveData.data(), 0x8000);
        saveFile.seekp(0x28800);
        saveFile.write(saveData.data(), 0x20000);
        fixEndianness((uint *)saveData.data(), 0x8000);
        return;
    }
    case SaveFormat::MemPak: {
        saveFile.seekp(0x800);
        saveFile.write(saveData.data(), 0x20000);
        return;
    }
    default: {
        fixEndianness((uint *)&saveData.data()[0x20800], 0xA000);
        saveFile.seekp(0);
        saveFile.write(saveData.data(), 0x48800);
        fixEndianness((uint *)&saveData.data()[0x20800], 0xA000);
        return;
    }
    }
}

static inline int bitcount(uint flags) {
#ifdef _WIN32
    int count = 0;
    while (flags != 0) {
        flags &= flags - 1;
        count++;
    }
    return count;
#else
    return __builtin_popcount(flags);
#endif
}

static uint countStarsForSlot(const StarLayout &layout, const Buffer &saveData,
                              uint slotIndex) {
    uint stars = 0;
    for (const StarLayout::Group &group : layout.groups) {
        for (const StarLayout::Course &course : group.courses) {
            for (const StarLayout::StarData &starData : course.stars) {
                const ubyte saveByte =
                    saveData.data()[layout.slotsStart + (slotIndex * layout.slotSize) +
                                    starData.offset];
                stars += (uint)bitcount((uint)(saveByte & starData.mask));
            }
        }
    }

    return stars;
}

uint StarLayout::countStars(const fs::path &saveFilePath) const {
    InputFile saveFile(saveFilePath, true);
    Buffer saveData = getUsedSaveData(saveFile);

    if (FileController::loadRhdcSettings().checkAllSaveSlots) {
        uint stars = 0;
        for (uint i = 0; i < (uint)numSlots; i++) {
            const uint slotStars = countStarsForSlot(*this, saveData, i);
            if (slotStars > stars) {
                stars = slotStars;
            }
        }
        return stars;
    } else {
        return countStarsForSlot(*this, saveData, 0);
    }
}

StarLayout StarLayout::createDefault() {
    return StarLayout{/* numSlots */ 4u,
                      /* slotsStart */ 0u,
                      /* slotSize */ 112u,
                      /* activeBit */ 95u,
                      /* checksumOffset */ 54u,
                      /* saveFormat */ SaveFormat::EEPROM,
                      /* groups */
                      {{"Main Courses",
                        StarLayout::Side::Left,
                        {{"1", {{12u, 0x7Fu}}},
                         {"2", {{13u, 0x7Fu}}},
                         {"3", {{14u, 0x7Fu}}},
                         {"4", {{15u, 0x7Fu}}},
                         {"5", {{16u, 0x7Fu}}},
                         {"6", {{17u, 0x7Fu}}},
                         {"7", {{18u, 0x7Fu}}},
                         {"8", {{19u, 0x7Fu}}},
                         {"9", {{20u, 0x7Fu}}},
                         {"10", {{21u, 0x7Fu}}},
                         {"11", {{22u, 0x7Fu}}},
                         {"12", {{23u, 0x7Fu}}},
                         {"13", {{24u, 0x7Fu}}},
                         {"14", {{25u, 0x7Fu}}},
                         {"15", {{26u, 0x7Fu}}}}},
                       {"Secret Courses",
                        StarLayout::Side::Right,
                        {{"A", {{8u, 0x7Fu}}},
                         {"B", {{27u, 0x7Fu}}},
                         {"C", {{28u, 0x7Fu}}},
                         {"D", {{29u, 0x7Fu}}},
                         {"E", {{30u, 0x7Fu}}},
                         {"F", {{31u, 0x7Fu}}},
                         {"G", {{32u, 0x7Fu}}},
                         {"H", {{33u, 0x7Fu}}},
                         {"I", {{34u, 0x7Fu}}},
                         {"J", {{35u, 0x7Fu}}},
                         {"K", {{36u, 0x7Fu}}}}}},
                      /* collectedStarIcon */ std::vector<ubyte>(),
                      /* missingStarIcon */ std::vector<ubyte>()};
}

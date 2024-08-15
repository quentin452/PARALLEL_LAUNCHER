#ifndef SRC_CORE_SDCARD_HPP_
#define SRC_CORE_SDCARD_HPP_

#include <string>
#include <vector>
#include <memory>
#include "src/polyfill/process.hpp"
#include "src/core/filesystem.hpp"

enum class SdCardFormat : int {
	FAT12 = 0,
	FAT16 = 1,
	FAT32 = 2,
	None = 3
};

struct SdCardInfo {
	std::string name;
	SdCardFormat format;
	uint numBlocks;
};

class MountedSdCard {

	private:
	const std::string m_path;
	const std::string m_device;
#ifdef _WIN32
	void *m_process;
	SdCardInfo m_info;
#endif
#ifdef __linux__
	const int m_fd;
#endif
	bool m_mounted;

	public:
	MountedSdCard(
		const std::string &mountDirectory,
		const std::string &devicePath
#ifdef __linux__
		, int fd
#endif
#ifdef _WIN32
		, void *process
		, SdCardInfo &&info
#endif
	) :
		m_path( mountDirectory ),
		m_device( devicePath ),
#ifdef __linux__
		m_fd( fd ),
#endif
#ifdef _WIN32
		m_process( process ),
		m_info( info ),
#endif
		m_mounted( true )
	{}
	~MountedSdCard();

	MountedSdCard( const MountedSdCard &other ) = delete;
	MountedSdCard( MountedSdCard &&other ) = delete;

#ifdef _WIN32
	const SdCardInfo &info() const { return m_info; }
#endif

	inline const std::string &path() const { return m_path; }
	bool unmount();

};

namespace SdCardManager {

	extern std::vector<SdCardInfo> fetchAll();
	extern bool tryFetch( const std::string &name, SdCardInfo &info );

	extern bool deleteCard( const std::string &name );
	extern AsyncProcess createCardAsync( const SdCardInfo &info );
	extern bool copyCard( const std::string &source, const std::string &target );

	extern bool canHardLink( const fs::path &filePath );
	extern std::string tryReadVolumeName( const fs::path &filePath );
	extern bool import( const fs::path &filePath, const std::string &name, bool copy );

	extern std::shared_ptr<MountedSdCard> mount( const std::string &name );

#ifdef _WIN32
	extern void recoverSdCards();
#endif

}

#endif /* SRC_CORE_SDCARD_HPP_ */

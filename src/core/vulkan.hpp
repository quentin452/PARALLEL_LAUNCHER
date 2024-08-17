#ifndef SRC_CORE_VULKAN_HPP_
#define SRC_CORE_VULKAN_HPP_

#include "src/core/version.hpp"
#include <initializer_list>
#include <unordered_set>

enum class GpuType {
  Unknown = 0,
  Integrated = 1,
  Discrete = 2,
  Virtual = 3,
  Software = 4
};

struct VulkanDeviceInfo {
  string gpuName;
  GpuType gpuType;
  uint vendorId;
  Version apiVersion;
  uint driverVersion;
  HashSet<string> supportedExtensions;

  bool supportsParaLLEl() const;
};

class VulkanInfo {

public:
  const bool hasVulkanApi;
  const HashSet<string> apiExtensions;
  const std::vector<VulkanDeviceInfo> devices;

private:
  inline VulkanInfo(bool _hasVulkanApi, HashSet<string> &&_apiExtensions,
                    std::vector<VulkanDeviceInfo> &&_devices)
      : hasVulkanApi(_hasVulkanApi), apiExtensions(std::move(_apiExtensions)),
        devices(std::move(_devices)) {}

  inline ~VulkanInfo() {}

  static VulkanInfo init();

public:
  static const VulkanInfo &get();

  inline static bool supportsParaLLEl() {
    for (const VulkanDeviceInfo &device : get().devices) {
      if (device.supportsParaLLEl())
        return true;
    }
    return false;
  }

  void serialize(
      JsonWriter &jw,
      const std::initializer_list<const char *> &relevantInstanceExtensions,
      const std::initializer_list<const char *> &relevantDeviceExtensions)
      const;
};

#endif /* SRC_CORE_VULKAN_HPP_ */

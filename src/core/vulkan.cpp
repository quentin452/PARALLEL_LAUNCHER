#include "src/core/vulkan.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <iomanip>
#include <sstream>
#include <locale>
#include "src/thirdparty/vulkan_core.h"

struct VulkanApi {
	VkInstance instance;
	PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
	PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
	PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
	PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
};

static inline void* getVulkanApiLoader() {
#if defined(_WIN32)
	HMODULE library = LoadLibraryA( "vulkan-1.dll" );
	if( !library ) return nullptr;
	return GetProcAddress( library, "vkGetInstanceProcAddr" );
#elif defined(__APPLE__)
	void *library = dlopen( "libvulkan.dylib", RTLD_NOW | RTLD_LOCAL );
	if( !library ) library = dlopen( "libvulkan.1.dylib", RTLD_NOW | RTLD_LOCAL );
	if( !library ) library = dlopen( "libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL );
	if( !library ) return nullptr;
	return dlsym( library, "vkGetInstanceProcAddr" );
#else
	void *library = dlopen( "libvulkan.so.1", RTLD_NOW | RTLD_LOCAL );
	if( !library ) library = dlopen( "libvulkan.so", RTLD_NOW | RTLD_LOCAL );
	if( !library ) return nullptr;
	return dlsym( library, "vkGetInstanceProcAddr" );
#endif
}

static inline bool tryGetVulkanApi( VulkanApi &api ) noexcept {
	PFN_vkGetInstanceProcAddr loader = (PFN_vkGetInstanceProcAddr)getVulkanApiLoader();
	if( !loader ) return false;

	const PFN_vkCreateInstance instanceFactory = (PFN_vkCreateInstance)loader( nullptr, "vkCreateInstance" );
	if( !instanceFactory ) return false;

	VkInstanceCreateInfo instanceInfo;
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pNext = nullptr;
	instanceInfo.flags = 0;
	instanceInfo.pApplicationInfo = nullptr;
	instanceInfo.enabledLayerCount = 0;
	instanceInfo.enabledExtensionCount = 0;

	if( instanceFactory( &instanceInfo, nullptr, &api.instance ) != VK_SUCCESS ) {
		return false;
	}

	api.vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)loader( api.instance, "vkEnumeratePhysicalDevices" );
	api.vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)loader( api.instance, "vkGetPhysicalDeviceProperties" );
	api.vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)loader( api.instance, "vkEnumerateInstanceExtensionProperties" );
	api.vkEnumerateDeviceExtensionProperties = (PFN_vkEnumerateDeviceExtensionProperties)loader( api.instance, "vkEnumerateDeviceExtensionProperties" );

	return
		api.vkEnumeratePhysicalDevices &&
		api.vkGetPhysicalDeviceProperties &&
		api.vkEnumerateInstanceExtensionProperties &&
		api.vkEnumerateDeviceExtensionProperties;
}

VulkanInfo VulkanInfo::init() {
	VulkanApi api;
	if( !tryGetVulkanApi( api ) ) {
		return VulkanInfo( false, HashSet<string>(), std::vector<VulkanDeviceInfo>() );
	}

	uint32_t numApiExtensions;
	HashSet<string> apiExtensions;
	
	if( api.vkEnumerateInstanceExtensionProperties( nullptr, &numApiExtensions, nullptr ) == VK_SUCCESS ) {
		std::vector<VkExtensionProperties> extensions( numApiExtensions );	
		if( api.vkEnumerateInstanceExtensionProperties( nullptr, &numApiExtensions, extensions.data() ) == VK_SUCCESS ) {
			apiExtensions.reserve( numApiExtensions );
			for( uint32_t i = 0; i < numApiExtensions; i++ ) {
				apiExtensions.insert( extensions[i].extensionName );
			}
		}
	}

	uint32_t numGpus;
	if( api.vkEnumeratePhysicalDevices( api.instance, &numGpus, nullptr ) != VK_SUCCESS ) {
		return VulkanInfo( true, std::move( apiExtensions ), std::vector<VulkanDeviceInfo>() );
	}

	std::vector<VkPhysicalDevice> gpus( numGpus );
	if( api.vkEnumeratePhysicalDevices( api.instance, &numGpus, gpus.data() ) != VK_SUCCESS ) {
		return VulkanInfo( true, std::move( apiExtensions ), std::vector<VulkanDeviceInfo>() );
	}

	std::vector<VulkanDeviceInfo> devices;
	devices.reserve( numGpus );
	for( uint32_t i = 0; i < numGpus; i++ ) {
		VkPhysicalDevice &gpu = gpus[i];
		VulkanDeviceInfo info;

		VkPhysicalDeviceProperties props;
		api.vkGetPhysicalDeviceProperties( gpu, &props );

		info.gpuName = props.deviceName;
		info.gpuType = (GpuType)props.deviceType;
		info.vendorId = props.vendorID;
		info.apiVersion = Version{
			(ushort)VK_API_VERSION_MAJOR(props.apiVersion),
			(ushort)VK_API_VERSION_MINOR(props.apiVersion),
			(ushort)VK_API_VERSION_PATCH(props.apiVersion)
		};
		info.driverVersion = props.driverVersion;

		uint32_t numExtensions;
		if( api.vkEnumerateDeviceExtensionProperties( gpu, nullptr, &numExtensions, nullptr ) == VK_SUCCESS ) {
			std::vector<VkExtensionProperties> extensions( numExtensions );
			if( api.vkEnumerateDeviceExtensionProperties( gpu, nullptr, &numExtensions, extensions.data() ) == VK_SUCCESS ) {
				info.supportedExtensions.reserve( numExtensions );
				for( uint32_t j = 0; j < numExtensions; j++ ) {
					info.supportedExtensions.insert( extensions[j].extensionName );
				}
			}
		}

		devices.emplace_back( std::move( info ) );
	}

	return VulkanInfo( true, std::move( apiExtensions ), std::move( devices ) );
}

const VulkanInfo &VulkanInfo::get() {
	static const VulkanInfo info = init();
	return info;
}

bool VulkanDeviceInfo::supportsParaLLEl() const {
	return supportedExtensions.count( VK_KHR_SWAPCHAIN_EXTENSION_NAME ) > 0;
}

static inline string gpuTypeString( GpuType type ) {
	switch( type ) {
		case GpuType::Integrated: return "Integrated";
		case GpuType::Discrete: return "Discrete";
		case GpuType::Virtual: return "Virtual";
		case GpuType::Software: return "Software";
		default: return "Unknown";
	}
}

static inline string hexWordString( uint word ) {
	std::stringstream stream;
	stream.imbue( std::locale::classic() );
	stream << "0x" << std::uppercase << std::hex << std::setfill( '0' ) << std::setw( 8 ) << word;
	return stream.str();
}

void VulkanInfo::serialize(
	JsonWriter &jw,
	const std::initializer_list<const char*> &relevantInstanceExtensions,
	const std::initializer_list<const char*> &relevantDeviceExtensions
) const {
	if( !hasVulkanApi ) {
		jw.writeNull();
		return;
	}

	jw.writeObjectStart();
	jw.writePropertyName( "relevantExtensions" );
	jw.writeObjectStart();
	for( const char *ext : relevantInstanceExtensions ) {
		jw.writeProperty( ext, apiExtensions.count( ext ) > 0 );
	}
	jw.writeObjectEnd();
	jw.writePropertyName( "devices" );
	jw.writeArrayStart();
	for( const VulkanDeviceInfo &device : devices ) {
		jw.writeObjectStart();
		jw.writeProperty( "name", device.gpuName );
		jw.writeProperty( "type", gpuTypeString( device.gpuType ) );
		jw.writeProperty( "vendorId", hexWordString( device.vendorId ) );
		jw.writeProperty( "apiVersion", device.apiVersion.toString() );
		jw.writeProperty( "driverVersion", hexWordString( device.driverVersion ) );
		jw.writePropertyName( "relevantExtensions" );
		jw.writeObjectStart();
		for( const char *ext : relevantDeviceExtensions ) {
			jw.writeProperty( ext, device.supportedExtensions.count( ext ) > 0 );
		}
		jw.writeObjectEnd();
		jw.writeObjectEnd();
	}
	jw.writeArrayEnd();
	jw.writeObjectEnd();
}

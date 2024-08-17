#include "src/core/gpu-telemetry.hpp"

#include "src/core/file-controller.hpp"
#include "src/core/opengl.hpp"
#include "src/core/version.hpp"
#include "src/core/vulkan.hpp"
#include "src/thirdparty/vulkan_core.h"
#include <sstream>

void GpuTelemetry::generateReport(JsonWriter &jw) {
  const std::optional<OpenGlInfo> openGlInfo = OpenGlInfo::getForActiveDevice();
  const VulkanInfo &vulkanInfo = VulkanInfo::get();
  const InstalledVersionsInfo &versionInfo =
      FileController::loadInstalledVersions();

  jw.writeObjectStart();
  jw.writeProperty("launcherVersion", CurrentVersion::Application.toString());
  jw.writeProperty("emulatorVersion", versionInfo.parallelVersion.toString());
#ifndef FLATPAK_VERSION
  jw.writeProperty("retroarchVersion",
                   versionInfo.retroArchVersion.version.toString());
#endif

  jw.writePropertyName("openGlInfo");
  if (openGlInfo.has_value()) {
    openGlInfo.value().serialize(
        jw,
        {/* relevant OpenGL extensions */
         "GL_ARB_buffer_storage", "GL_ARB_copy_image",
         "GL_ARB_fragment_shader_interlock", "GL_ARB_get_program_binary",
         "GL_ARB_shader_image_load_store", "GL_ARB_texture_barrier",
         "GL_ARB_texture_non_power_of_two", "GL_ARB_texture_storage",
         "GL_ARM_shader_framebuffer_fetch_depth_stencil",
         "GL_EXT_buffer_storage", "GL_EXT_copy_image",
         "GL_EXT_shader_framebuffer_fetch", "GL_EXT_texture_env_add",
         "GL_EXT_texture_filter_anistropic", "GL_EXT_texture_format_BGRA8888",
         "GL_INTEL_fragment_shader_ordering", "GL_NV_fragment_shader_interlock",
         "GL_NV_shader_noperspective_interpolation", "GL_NV_texture_barrier"});
  } else {
    jw.writeNull();
  }

  jw.writePropertyName("vulkanInfo");
  vulkanInfo.serialize(
      jw,
      {/* relevant Vulkan instance extensions */
       VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
       VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,
       VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
       VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
       VK_KHR_SURFACE_EXTENSION_NAME},
      {/* relevant Vulkan device extensions */
       VK_KHR_SWAPCHAIN_EXTENSION_NAME,
       VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
       VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME,
       VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
       VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME,
#ifdef _WIN32
       "VK_EXT_full_screen_exclusive"
#endif
       VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME,
       VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
       VK_EXT_SHADER_DEMOTE_TO_HELPER_INVOCATION_EXTENSION_NAME,
       VK_EXT_SUBGROUP_SIZE_CONTROL_EXTENSION_NAME,
       VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME,
       VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
       VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
       VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
       VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
       VK_KHR_DESCRIPTOR_UPDATE_TEMPLATE_EXTENSION_NAME,
       VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
       VK_KHR_DRIVER_PROPERTIES_EXTENSION_NAME,
       VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
       VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
       VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
       VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
       VK_KHR_MAINTENANCE1_EXTENSION_NAME,
       VK_KHR_MAINTENANCE2_EXTENSION_NAME,
       VK_KHR_MAINTENANCE3_EXTENSION_NAME,
       VK_KHR_MULTIVIEW_EXTENSION_NAME,
       VK_KHR_PERFORMANCE_QUERY_EXTENSION_NAME,
       VK_KHR_SAMPLER_MIRROR_CLAMP_TO_EDGE_EXTENSION_NAME,
       VK_KHR_SAMPLER_YCBCR_CONVERSION_EXTENSION_NAME,
       VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
       VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
       VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
       VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
       VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
       VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME});

  jw.writeObjectEnd();
}

string GpuTelemetry::generateReport() {
  std::ostringstream stream;
  JsonWriter writer(&stream, true);
  generateReport(writer);
  return stream.str();
}

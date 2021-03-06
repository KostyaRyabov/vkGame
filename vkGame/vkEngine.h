#pragma once

#include "Settings.h"

static uint8_t	step_counter = 0;
#define init_step(stp) if (stp) step_counter++; else return
#define deinit_step(stp) if (5 - step_counter > 0) step_counter--; else stp

#define vk_assert(arg) if (arg != VK_SUCCESS) return false
#define vk_check(arg) if (!arg) return false
#define vk_verify(arg) (arg == VK_SUCCESS)

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  objectType,
	uint64_t                    object,
	size_t                      location,
	int32_t                     messageCode,
	const char*					pLayerPrefix,
	const char*					pMessage,
	void*						pUserData)
{
	std::cout << "[VK_DEBUG_REPORT] ";
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
		std::cout << "ERROR";
	}
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
		std::cout << "WARNING";
	}
	else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
		std::cout << "INFORMATION";
	}
	else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
		std::cout << "PERFORMANCE";
	}
	else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
		std::cout << "DEBUG";
	}
	else {
		return VK_FALSE;
	}
	std::cout << ": [" << pLayerPrefix << "] Code" << messageCode << ":" << pMessage << std::endl;
	return VK_TRUE;
}


class vkEngine
{
public:
	vkEngine();
	~vkEngine();

	void Run();
private:
	VkInstance								instance								=		VK_NULL_HANDLE;

	VkDebugReportCallbackEXT				debugReportCallback;
	PFN_vkCreateDebugReportCallbackEXT		dbgCreateDebugReportCallback;
	PFN_vkDebugReportMessageEXT				dbgDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT		dbgDestroyDebugReportCallback;

	VkPhysicalDevice						physical_device							=		VK_NULL_HANDLE;
	VkPhysicalDeviceProperties				physical_device_properties;
	VkPhysicalDeviceFeatures				physical_device_features;
	VkPhysicalDeviceMemoryProperties		physical_device_memory_properties;

	VkDevice								device									=		VK_NULL_HANDLE;

	VkQueue									queue;
	uint32_t								graphics_queue_family_index				=		0;
	uint32_t								command_family_index					=		0;

	VkSurfaceKHR							surface;
	VkSurfaceCapabilitiesKHR				surface_capabilities;
	
	std::vector<VkSurfaceFormatKHR>			supported_surface_format_list;
	std::vector<VkPresentModeKHR>			supported_presentation_mode_list;

	uint32_t								amountOfImagesInSwapchain;
	VkSwapchainKHR							swapchain;
	Window									window;

	VkImageView*							image_view_list;

	VkShaderModule							shader_module_vert;
	VkShaderModule							shader_module_frag;
	VkPipelineLayout						pipeline_layout							=		VK_NULL_HANDLE;
	VkRenderPass							render_pass;
	VkPipeline								pipeline;
	VkFramebuffer*							frame_buffer_list;

	VkCommandPool							command_pool;
	VkCommandBuffer*						command_buffer_list;

	VkSemaphore								semaphore_image_available;
	VkSemaphore								semaphore_rendering_done;

	//------------

	bool InitInstance();
	void DeinitInstance();

	bool InitDebugReport();
	void DeinitDebugReport();

	bool InitDevice();
	void DeinitDevice();

	bool InitQueue();
	void DeinitQueue();

	bool InitSurface();
	void DeinitSurface();

	bool InitSwapchain();
	void DeinitSwapchain();

	bool InitSwapchainImages();
	void DeinitSwapchainImages();

	bool InitPipelines();
	void DeinitPipelines();

	bool InitFramebuffer();
	void DeinitFramebuffer();

	bool InitCommandPool();
	void DeinitCommandPool();

	//-------------

	bool CreateShaderModule(const char* filename, VkShaderModule* shader_module);
	bool DrawFrame();
	bool DeviceExtensionsSupport(const VkPhysicalDevice& pysicalDevice, std::vector<const char*>& requiredExtensions);
	bool GetQueueFamily(const VkPhysicalDevice& physicalDevice, int flags, uint32_t& returnedFamilyIndex);
};


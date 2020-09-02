#pragma once

#include "pch.h"

#define init_step(stp) if (stp) step_counter++; else return
#define deinit_step(stp) if (5 - step_counter > 0) step_counter--; else stp

#define vk_assert(arg) if (arg != VK_SUCCESS) return false
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
	VkInstance instance = VK_NULL_HANDLE;

	VkDebugReportCallbackEXT debugReportCallback;
	PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
	PFN_vkDebugReportMessageEXT dbgDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;

	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties physical_device_properties;
	VkPhysicalDeviceFeatures physical_device_features;
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;

	VkDevice device = VK_NULL_HANDLE;

	VkQueue queue;
	uint32_t graphic_queue_family_index = 0;

	VkSurfaceKHR surface;

	Window window;

	uint8_t step_counter = 0;

	bool InitInstance();
	void DeinitInstance();

	bool InitDebugReport();
	void DeinitDebugReport();

	bool InitDevice();
	void DeinitDevice();

	bool InitQueue();
	void DeinitQueue();

	bool InitWindow();
	void DeinitWindow();

	bool InitSurface();
	void DeinitSurface();

	//-------------

	bool DeviceExtensionsSupport(const VkPhysicalDevice& pysicalDevice, const char** requiredExtensions, uint32_t extensionCount);
	bool GetQueueFamily(const VkPhysicalDevice& pysicalDevice, int flags, uint32_t& returnedFamilyIndex);
};


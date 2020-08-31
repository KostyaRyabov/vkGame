#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include "pch.h"

#define check_step(stp) if (stp) step_counter++; else return
#define vk_verify(arg) (arg == VK_SUCCESS)
#define vk_assert(arg) if (arg != VK_SUCCESS) return false


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
private:
	VkDebugReportCallbackEXT debugReportCallback;
	PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
	PFN_vkDebugReportMessageEXT dbgDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;

	VkInstance instance;

	VkPhysicalDevice physical_device;
	VkPhysicalDeviceProperties physical_device_properties;
	VkPhysicalDeviceFeatures physical_device_features;
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;

	VkDevice device;

	VkQueue queue;

	VkSurfaceKHR surface;

	HINSTANCE hInstance = GetModuleHandle(nullptr);

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
};


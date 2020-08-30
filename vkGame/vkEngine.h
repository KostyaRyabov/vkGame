#pragma once

#include "pch.h"

#define vk_verify(arg) (arg == VK_SUCCESS)
#define vk_assert(arg) if (arg != VK_SUCCESS) return false

class vkEngine
{
public:
	vkEngine();
	~vkEngine();
private:
	VkInstance instance;

	VkPhysicalDevice physical_device;
	VkPhysicalDeviceProperties physical_device_properties;
	VkPhysicalDeviceFeatures physical_device_features;
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;

	VkDevice device;

	uint8_t step_counter = 0;
	void check_step(bool stp);

	bool InitInstance();
	void DeinitInstance();

	bool InitDevice();
	void DeinitDevice();
};


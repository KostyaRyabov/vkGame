#pragma once
#include "vulkan/vulkan.h"

class vkEngine
{
public:
	VkInstance instance;
	VkApplicationInfo app_info = {};
	VkInstanceCreateInfo instance_create_info = {};

	vkEngine();
	~vkEngine();
};


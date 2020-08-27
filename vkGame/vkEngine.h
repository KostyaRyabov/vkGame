#pragma once

#ifdef _DEBUG
#include "Tools.h"
#define verify(res) \
	Tools::Debug::Log(#res); \
	return Tools::Debug::check(res)
#else
#include "pch.h"
#define verify(res) return true
#endif

class vkEngine
{
public:
	vkEngine();
	~vkEngine();
private:
	VkInstance instance;
	VkApplicationInfo app_info = {};
	VkInstanceCreateInfo instance_create_info = {};

	bool InitInstance();
	void DeinitInstance();



};


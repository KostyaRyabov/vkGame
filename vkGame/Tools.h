#pragma once

#include "pch.h"

namespace Tools {
	namespace Debug {
		static std::fstream LogFile("log.txt", std::fstream::in | std::fstream::app);

		void Log(std::string msg) {
			std::cout << msg << std::endl;
			LogFile << msg;
		}

		bool check(VkResult vk_res) {
			switch (vk_res)
			{
			case VK_SUCCESS:
				Log("	VK_SUCCESS");
				return true;
			case VK_NOT_READY:
				Log("	VK_NOT_READY");
				break;
			case VK_TIMEOUT:
				Log("	VK_TIMEOUT");
				break;
			case VK_EVENT_SET:
				Log("	VK_EVENT_SET");
				break;
			case VK_EVENT_RESET:
				Log("	VK_EVENT_RESET");
				break;
			case VK_INCOMPLETE:
				Log("	VK_INCOMPLETE");
				break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				Log("	VK_ERROR_OUT_OF_HOST_MEMORY");
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				Log("	VK_ERROR_OUT_OF_DEVICE_MEMORY");
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				Log("	VK_ERROR_INITIALIZATION_FAILED");
				break;
			case VK_ERROR_DEVICE_LOST:
				Log("	VK_ERROR_DEVICE_LOST");
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				Log("	VK_ERROR_MEMORY_MAP_FAILED");
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				Log("	VK_ERROR_LAYER_NOT_PRESENT");
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				Log("	VK_ERROR_EXTENSION_NOT_PRESENT");
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				Log("	VK_ERROR_FEATURE_NOT_PRESENT");
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				Log("	VK_ERROR_INCOMPATIBLE_DRIVER");
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				Log("	VK_ERROR_TOO_MANY_OBJECTS");
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				Log("	VK_ERROR_FORMAT_NOT_SUPPORTED");
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				Log("	VK_ERROR_FRAGMENTED_POOL");
				break;
			case VK_ERROR_UNKNOWN:
				Log("	VK_ERROR_UNKNOWN");
				break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				Log("	VK_ERROR_OUT_OF_POOL_MEMORY");
				break;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				Log("	VK_ERROR_INVALID_EXTERNAL_HANDLE");
				break;
			case VK_ERROR_FRAGMENTATION:
				Log("	VK_ERROR_FRAGMENTATION");
				break;
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
				Log("	VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS");
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				Log("	VK_ERROR_SURFACE_LOST_KHR");
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				Log("	VK_ERROR_NATIVE_WINDOW_IN_USE_KHR");
				break;
			case VK_SUBOPTIMAL_KHR:
				Log("	VK_SUBOPTIMAL_KHR");
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				Log("	VK_ERROR_OUT_OF_DATE_KHR");
				break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				Log("	VK_ERROR_INCOMPATIBLE_DISPLAY_KHR");
				break;
			case VK_ERROR_VALIDATION_FAILED_EXT:
				Log("	VK_ERROR_VALIDATION_FAILED_EXT");
				break;
			case VK_ERROR_INVALID_SHADER_NV:
				Log("	VK_ERROR_INVALID_SHADER_NV");
				break;
			case VK_ERROR_INCOMPATIBLE_VERSION_KHR:
				Log("	VK_ERROR_INCOMPATIBLE_VERSION_KHR");
				break;
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
				Log("	VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT");
				break;
			case VK_ERROR_NOT_PERMITTED_EXT:
				Log("	VK_ERROR_NOT_PERMITTED_EXT");
				break;
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
				Log("	VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT");
				break;
			case VK_THREAD_IDLE_KHR:
				Log("	VK_THREAD_IDLE_KHR");
				break;
			case VK_THREAD_DONE_KHR:
				Log("	VK_THREAD_DONE_KHR");
				break;
			case VK_OPERATION_DEFERRED_KHR:
				Log("	VK_OPERATION_DEFERRED_KHR");
				break;
			case VK_OPERATION_NOT_DEFERRED_KHR:
				Log("	VK_OPERATION_NOT_DEFERRED_KHR");
				break;
			case VK_PIPELINE_COMPILE_REQUIRED_EXT:
				Log("	VK_PIPELINE_COMPILE_REQUIRED_EXT");
				break;
			case VK_RESULT_MAX_ENUM:
				Log("	VK_RESULT_MAX_ENUM");
				break;
			default:
				Log("	 --- ");
				break;
			}

			return false;
		}
	}
}
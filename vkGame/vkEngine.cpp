#include "vkEngine.h"

vkEngine::vkEngine() {
    init_step(InitInstance());
    init_step(InitDebugReport());
    init_step(InitDevice());
    init_step(window.Create(800, 480, L"_test_"));
    init_step(InitSurface());

    window.Open();
    
    init_step(InitQueue());
    init_step(InitSwapchain());

    init_step(InitSwapchainImages());

    Run();
}

vkEngine::~vkEngine() {
    vkDeviceWaitIdle(device);

    deinit_step(DeinitSwapchainImages());
    deinit_step(DeinitSwapchain());
    deinit_step(DeinitQueue());
    deinit_step(DeinitSurface());
    deinit_step(window.Close());
    deinit_step(DeinitDevice());
    deinit_step(DeinitDebugReport());
    deinit_step(DeinitInstance());
}

bool vkEngine::InitInstance()
{
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = "GAME";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = VK_API_VERSION_1_2;

    const uint8_t amountOfLayers = 3;
    const char* layers[amountOfLayers] = {
        "VK_LAYER_LUNARG_standard_validation",
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_AMD_switchable_graphics"
    };

    const uint8_t amountOfExtensions = 3;
    const char* extensions[amountOfExtensions] = {
        "VK_EXT_debug_report",
        "VK_KHR_surface",
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    {
        uint32_t amountOfAvailableLayers = 0;
        vk_assert(vkEnumerateInstanceLayerProperties(&amountOfAvailableLayers, nullptr));
        VkLayerProperties* available_layers = new VkLayerProperties[amountOfAvailableLayers];
        vk_assert(vkEnumerateInstanceLayerProperties(&amountOfAvailableLayers, available_layers));
        
        uint32_t amountOfAvailableExtensions = 0;
        vk_assert(vkEnumerateInstanceExtensionProperties(nullptr, &amountOfAvailableExtensions, nullptr));
        VkExtensionProperties* available_extensions = new VkExtensionProperties[amountOfAvailableExtensions];
        vk_assert(vkEnumerateInstanceExtensionProperties(nullptr, &amountOfAvailableExtensions, available_extensions));
        
        {
            for (uint32_t i = 0; i < amountOfAvailableLayers; i++) {
                std::cout << "layer #" << i << std::endl;

                std::cout << "layer_name        : " << available_layers[i].layerName << std::endl;
                std::cout << "layer_description : " << available_layers[i].description << std::endl << std::endl;
            }

            for (uint32_t i = 0; i < amountOfAvailableExtensions; i++) {
                std::cout << "extansion #" << i << std::endl;

                std::cout << "extansion_name : " << available_extensions[i].extensionName << std::endl << std::endl;
            }
        }

        {
            bool founded;

            for (int i = 0; i < amountOfLayers; i++) {
                founded = false;

                for (int j = 0; j < amountOfAvailableLayers; j++) {
                    if (!strcmp(available_layers[j].layerName, layers[i])) {
                        founded = true;
                        break;
                    }
                }

                if (!founded) return false;
            }

            for (int i = 0; i < amountOfExtensions; i++) {
                founded = false;

                for (int j = 0; j < amountOfAvailableExtensions; j++) {
                    if (!strcmp(available_extensions[j].extensionName, extensions[i])) {
                        founded = true;
                        break;
                    }
                }

                if (!founded) {
                    delete[] available_layers;
                    delete[] available_extensions;
                    return false;
                }
            }
        }

        delete[] available_layers;
        delete[] available_extensions;
    }

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = NULL;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledLayerCount = amountOfLayers;
    instance_create_info.ppEnabledLayerNames = layers;
    instance_create_info.enabledExtensionCount = amountOfExtensions;
    instance_create_info.ppEnabledExtensionNames = extensions;
    
    return vk_verify(vkCreateInstance(&instance_create_info, nullptr, &instance));
}

void vkEngine::DeinitInstance()
{
    vkDestroyInstance(instance, nullptr);
}

bool vkEngine::InitDebugReport()
{
    dbgCreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (!dbgCreateDebugReportCallback) return vk_verify(VK_ERROR_INITIALIZATION_FAILED);

    dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (!dbgDestroyDebugReportCallback) return vk_verify(VK_ERROR_INITIALIZATION_FAILED);

    VkDebugReportCallbackCreateInfoEXT debug_report_callback_create_info;
    debug_report_callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_report_callback_create_info.pNext = nullptr;
    debug_report_callback_create_info.flags = 
        VK_DEBUG_REPORT_WARNING_BIT_EXT | 
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    debug_report_callback_create_info.pfnCallback = &DebugReportCallback;
    debug_report_callback_create_info.pUserData = nullptr;

    return vk_verify(dbgCreateDebugReportCallback(instance, &debug_report_callback_create_info, nullptr, &debugReportCallback));
}

void vkEngine::DeinitDebugReport()
{
    if (debugReportCallback) dbgDestroyDebugReportCallback(instance, debugReportCallback, NULL);
}

bool vkEngine::InitDevice()
{
    uint32_t amountOfPhysicalDevices = 0;
    vk_assert(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr));
    VkPhysicalDevice* physical_device_list = new VkPhysicalDevice[amountOfPhysicalDevices];
    vk_assert(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physical_device_list));

    // show Physical Device list
    for (uint32_t i = 0; i < amountOfPhysicalDevices; i++) {
        std::cout << "physical_device #" << i << std::endl;

        vkGetPhysicalDeviceProperties(physical_device_list[i], &physical_device_properties);
        vkGetPhysicalDeviceFeatures(physical_device_list[i], &physical_device_features);
        vkGetPhysicalDeviceMemoryProperties(physical_device_list[i], &physical_device_memory_properties);

        std::cout << "deviceName : " << physical_device_properties.deviceName << std::endl;
        std::cout << "apiVersion : "
            << VK_VERSION_MAJOR(physical_device_properties.apiVersion) << "."
            << VK_VERSION_MINOR(physical_device_properties.apiVersion) << "."
            << VK_VERSION_PATCH(physical_device_properties.apiVersion) << std::endl << std::endl;
    }

    uint32_t amountOfRequieredExtension = 1;
    const char* required_extension_list = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    for (uint32_t i = 0; i < amountOfPhysicalDevices; i++) {
        if (DeviceExtensionsSupport(physical_device_list[i], &required_extension_list, amountOfRequieredExtension)) {
            if (GetQueueFamily(physical_device_list[i], VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT | VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, graphics_queue_family_index)) {
                vkGetPhysicalDeviceFeatures(physical_device_list[i], &physical_device_features);
                vkGetPhysicalDeviceProperties(physical_device_list[i], &physical_device_properties);
                vkGetPhysicalDeviceMemoryProperties(physical_device_list[i], &physical_device_memory_properties);

                //поиск необходимых характеристик

                physical_device = physical_device_list[i];
            }
        }
    }

    delete[] physical_device_list;
    if (physical_device == VK_NULL_HANDLE) return false;

    float queue_priorities[] = {1.0f};  // add for multithreading

    VkDeviceQueueCreateInfo device_queue_create_info = {};
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.pNext = nullptr;
    device_queue_create_info.flags = 0;
    device_queue_create_info.queueFamilyIndex = graphics_queue_family_index;
    device_queue_create_info.queueCount = 1;
    device_queue_create_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = nullptr;
    device_create_info.flags = NULL;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = nullptr;
    device_create_info.enabledExtensionCount = amountOfRequieredExtension;
    device_create_info.ppEnabledExtensionNames = &required_extension_list;
    device_create_info.pEnabledFeatures = &physical_device_features;

    // Pick "best device"
    return vk_verify(vkCreateDevice(physical_device, &device_create_info, NULL, &device));
}

void vkEngine::DeinitDevice()
{
    vkDestroyDevice(device, nullptr);
}

bool vkEngine::InitQueue()
{
    vkGetDeviceQueue(device, 0, 0, &queue);
    return true;
}

void vkEngine::DeinitQueue()
{
}

bool vkEngine::InitSurface()
{
    VkWin32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = nullptr;
    surface_create_info.flags = NULL;
    surface_create_info.hinstance = window.GetInstance();
    surface_create_info.hwnd = window.GetHandle();

    return vk_verify(vkCreateWin32SurfaceKHR(instance, &surface_create_info, nullptr, &surface));
}

void vkEngine::DeinitSurface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

bool vkEngine::InitSwapchain()
{
    VkBool32 surfaceSupport = VK_FALSE;
    vk_assert(vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, graphics_queue_family_index, surface, &surfaceSupport));

    if (!surfaceSupport) {
        std::cout << "Surface is not supported!\n";
        return false;
    }

    {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_capabilities);

        amountOfSurfaceFormats = 0;
        vk_assert(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &amountOfSurfaceFormats, nullptr));
        supported_surface_format_list = new VkSurfaceFormatKHR[amountOfSurfaceFormats];
        vk_assert(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &amountOfSurfaceFormats, supported_surface_format_list));

        amountOfSurfacePresentationModes = 0;
        vk_assert(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &amountOfSurfacePresentationModes, nullptr));
        supported_presentation_mode_list = new VkPresentModeKHR[amountOfSurfacePresentationModes];
        vk_assert(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &amountOfSurfacePresentationModes, supported_presentation_mode_list));
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = nullptr;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = surface_capabilities.minImageCount;                   //
    swapchain_create_info.imageFormat = supported_surface_format_list[0].format;                // TODO: later find optimal
    swapchain_create_info.imageColorSpace = supported_surface_format_list[0].colorSpace;        //
    swapchain_create_info.imageExtent = window.GetSize();
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = nullptr;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = supported_presentation_mode_list[0];                    // TODO: later find optimal 
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    //delete[] surface_format_list;
    //delete[] presentation_mode_list;

    return vk_verify(vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain));
}

void vkEngine::DeinitSwapchain()
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

bool vkEngine::InitSwapchainImages()
{
    amountOfSwapchainImages = 0;
    vk_assert(vkGetSwapchainImagesKHR(device, swapchain, &amountOfSwapchainImages, nullptr));
    VkImage* swapchainImages = new VkImage[amountOfSwapchainImages];
    vk_assert(vkGetSwapchainImagesKHR(device, swapchain, &amountOfSwapchainImages, swapchainImages));

    imageViews = new VkImageView[amountOfSwapchainImages];

    for (uint32_t i = 0; i < amountOfSwapchainImages; i++) {
        VkImageViewCreateInfo imageView_create_info = {};
        imageView_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageView_create_info.pNext = nullptr;
        imageView_create_info.flags = 0;
        imageView_create_info.image = swapchainImages[0];
        imageView_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageView_create_info.format = supported_surface_format_list[0].format;
        imageView_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageView_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageView_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageView_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageView_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageView_create_info.subresourceRange.baseArrayLayer = 0;
        imageView_create_info.subresourceRange.baseMipLevel = 0;
        imageView_create_info.subresourceRange.layerCount = 1;
        imageView_create_info.subresourceRange.levelCount = 1;

        vk_assert(vkCreateImageView(device, &imageView_create_info, nullptr, &imageViews[i]));
    }

    
    return false;
}

void vkEngine::DeinitSwapchainImages()
{
    for (uint32_t i = 0; i < amountOfSwapchainImages; i++) {
        vkDestroyImageView(device, imageViews[i], nullptr);
    };

    delete[] imageViews;
}

//---------------

bool vkEngine::DeviceExtensionsSupport(const VkPhysicalDevice& pysicalDevice, const char** requiredExtensions, uint32_t extensionCount)
{
    uint32_t amountOfExtensions = 0;
    vk_assert(vkEnumerateDeviceExtensionProperties(pysicalDevice, nullptr, &amountOfExtensions, nullptr));
    VkExtensionProperties* extension_list = new VkExtensionProperties[amountOfExtensions];
    vk_assert(vkEnumerateDeviceExtensionProperties(pysicalDevice, nullptr, &amountOfExtensions, extension_list));

    bool founded;

    for (uint32_t i = 0; i < extensionCount; i++) {
        founded = false;

        for (uint32_t j = 0; j < amountOfExtensions; j++) {
            if (!strcmp(extension_list[j].extensionName, requiredExtensions[i])) {
                founded = true;
                break;
            }
        }

        if (!founded) {
            delete[] extension_list;
            return false;
        }
    }

    delete[] extension_list;
    return true;
}

bool vkEngine::GetQueueFamily(const VkPhysicalDevice& pysicalDevice, int flags, uint32_t& returnedFamilyIndex)
{
    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pysicalDevice, &amountOfQueueFamilies, nullptr);
    VkQueueFamilyProperties* queue_family_property_list = new VkQueueFamilyProperties[amountOfQueueFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(pysicalDevice, &amountOfQueueFamilies, queue_family_property_list);

    for (uint32_t i = 0; i < amountOfQueueFamilies; i++) {
        std::cout << "queue family #" << i << std::endl;

        std::cout << "queueCount : " << queue_family_property_list[i].queueCount << std::endl;

        std::cout << "VK_QUEUE_GRAPHICS_BIT         : " << ((queue_family_property_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT          : " << ((queue_family_property_list[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT         : " << ((queue_family_property_list[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT   : " << ((queue_family_property_list[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_PROTECTED_BIT        : " << ((queue_family_property_list[i].queueFlags & VK_QUEUE_PROTECTED_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_FLAG_BITS_MAX_ENUM   : " << ((queue_family_property_list[i].queueFlags & VK_QUEUE_FLAG_BITS_MAX_ENUM) != 0) << std::endl;

        std::cout << "minImageTransferGranularity   : ["
            << queue_family_property_list[i].minImageTransferGranularity.depth << "."
            << queue_family_property_list[i].minImageTransferGranularity.depth << "."
            << queue_family_property_list[i].minImageTransferGranularity.depth << "]" << std::endl << std::endl;
    }

    for (uint32_t i = 0; i < amountOfQueueFamilies; i++) {
        if (queue_family_property_list[i].queueCount > 0) {
            if ((queue_family_property_list[i].queueFlags & flags) == flags) {
                returnedFamilyIndex = i;
                delete[] queue_family_property_list;
                return true;
            }
        }
    }

    delete[] queue_family_property_list;
    return false;
}

void vkEngine::Run()
{
    while (window.Update()) {

    }
}

int main() {
    vkEngine engine;
    engine.Run();
    return 0;
}
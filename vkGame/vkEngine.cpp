#include "vkEngine.h"

vkEngine::vkEngine() {
    check_step(InitInstance());
    check_step(InitDebugReport());
    check_step(InitDevice());
    check_step(InitQueue());
    check_step(InitSurface());
}

vkEngine::~vkEngine() {
    vkDeviceWaitIdle(device);

    DeinitSurface();
    DeinitQueue();
    DeinitDevice();
    DeinitDebugReport();
    DeinitInstance();
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
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_AMD_switchable_graphics",
        "VK_LAYER_LUNARG_monitor"
    };

    const uint8_t amountOfExtensions = 3;
    const char* extensions[amountOfExtensions] = {
        "VK_EXT_debug_report",
        "VK_KHR_surface",
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    {
        uint32_t amountOfAvailableLayers = 0;
        vkEnumerateInstanceLayerProperties(&amountOfAvailableLayers, nullptr);
        VkLayerProperties* available_layers = new VkLayerProperties[amountOfAvailableLayers];
        vkEnumerateInstanceLayerProperties(&amountOfAvailableLayers, available_layers);

        uint32_t amountOfAvailableExtensions = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &amountOfAvailableExtensions, nullptr);
        VkExtensionProperties* available_extensions = new VkExtensionProperties[amountOfAvailableExtensions];
        vkEnumerateInstanceExtensionProperties(nullptr, &amountOfAvailableExtensions, available_extensions);

        /*
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
        */

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

                if (!founded) return false;
            }
        }
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
    if (!dbgCreateDebugReportCallback) {
        std::cout << "Error: GetInstanceProcAddr unable to locate vkCreateDebugReportCallbackEXT function.\n";
        return vk_verify(VK_ERROR_INITIALIZATION_FAILED);
    }
    std::cout << "GetInstanceProcAddr loaded dbgCreateDebugReportCallback function.\n";

    dbgDestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (!dbgDestroyDebugReportCallback) {
        std::cout << "Error: GetInstanceProcAddr unable to locate vkDestroyDebugReportCallbackEXT function.\n";
        return vk_verify(VK_ERROR_INITIALIZATION_FAILED);
    }
    std::cout << "GetInstanceProcAddr loaded dbgDestroyDebugReportCallback function.\n\n";


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
    if (debugReportCallback) {
        dbgDestroyDebugReportCallback(instance, debugReportCallback, NULL);
    }
}

bool vkEngine::InitDevice()
{
    uint32_t amountOfPhysicalDevices = 0;
    vk_assert(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr));
    VkPhysicalDevice* physical_device_list = new VkPhysicalDevice[amountOfPhysicalDevices];
    vk_assert(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physical_device_list));

    {
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
    }

    physical_device = physical_device_list[0];

    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &amountOfQueueFamilies, nullptr);
    VkQueueFamilyProperties* queue_family_property_list = new VkQueueFamilyProperties[amountOfQueueFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &amountOfQueueFamilies, queue_family_property_list);

    {
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
    }

    // check available queue families and continue ...

    float queue_priorities[] = {1.0f};

    VkDeviceQueueCreateInfo device_queue_create_info = {};
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.pNext = nullptr;
    device_queue_create_info.flags = 0;
    device_queue_create_info.queueFamilyIndex = 0;      //chouse correct family index
    device_queue_create_info.queueCount = 1;            //check if this amount is available
    device_queue_create_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = nullptr;
    device_create_info.flags = NULL;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = nullptr;
    device_create_info.enabledExtensionCount = 0;
    device_create_info.ppEnabledExtensionNames = nullptr;
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

bool vkEngine::InitWindow()
{
    
    return false;
}

bool vkEngine::InitSurface()
{
    VkWin32SurfaceCreateInfoKHR surface_create_info;
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = nullptr;
    surface_create_info.flags = NULL;
    surface_create_info.hinstance = nullptr;
    surface_create_info.hwnd = nullptr;

    return vk_verify(vkCreateWin32SurfaceKHR(instance, &surface_create_info, nullptr, &surface));
}

void vkEngine::DeinitSurface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

int main() {
    vkEngine engine;
    return 0;
}
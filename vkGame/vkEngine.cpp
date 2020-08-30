#include "vkEngine.h"

vkEngine::vkEngine() {
    check_step(InitInstance());
    check_step(InitDevice());
}

vkEngine::~vkEngine() {
    DeinitDevice();
    DeinitInstance();
}

void vkEngine::check_step(bool stp)
{
    if (stp) step_counter++;
    else return;
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
    
    // check available layers and extensions

    uint32_t amountOfLayers = 0;
    vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
    VkLayerProperties* layers = new VkLayerProperties[amountOfLayers];
    vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

    uint32_t amountOfExtansions = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtansions, nullptr);
    VkExtensionProperties* extensions = new VkExtensionProperties[amountOfExtansions];
    vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtansions, extensions);

#ifdef _DEBUG
    const uint8_t amountOfValidationLayers = 3;
    const char* validation_layers[amountOfValidationLayers] = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor",
        "VK_LAYER_AMD_switchable_graphics"
    };

    const uint8_t amountOfValidationExtensions = 3;
    const char* validation_extensions[amountOfValidationExtensions] = {
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_LUNARG_monitor",
        "VK_LAYER_AMD_switchable_graphics"
    };
#else
    const uint8_t amountOfValidationLayers = 1;
    const char* validation_layers[amountOfValidationLayers] = {
        "VK_LAYER_AMD_switchable_graphics"
};
#endif

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = NULL;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledLayerCount = amountOfValidationLayers;
    instance_create_info.ppEnabledLayerNames = validation_layers;
    instance_create_info.enabledExtensionCount = 0;
    instance_create_info.ppEnabledExtensionNames = nullptr;

    return vk_verify(vkCreateInstance(&instance_create_info, nullptr, &instance));
}

void vkEngine::DeinitInstance()
{
    vkDestroyInstance(instance, nullptr);
}

bool vkEngine::InitDevice()
{
    uint32_t amountOfPhysicalDevices = 0;
    vk_assert(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr));
    VkPhysicalDevice* physical_device_list = new VkPhysicalDevice[amountOfPhysicalDevices];
    vk_assert(vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physical_device_list));

    physical_device = physical_device_list[0];

    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
    vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);
    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &amountOfQueueFamilies, nullptr);
    VkQueueFamilyProperties* queue_family_property_list = new VkQueueFamilyProperties[amountOfQueueFamilies];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &amountOfQueueFamilies, queue_family_property_list);

    // check available queue families and continue ...

    float queue_priorities[] = {1.0f, 1.0f,1.0f, 1.0f};

    VkDeviceQueueCreateInfo device_queue_create_info = {};
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.pNext = nullptr;
    device_queue_create_info.flags = 0;
    device_queue_create_info.queueFamilyIndex = 0;      //chouse correct family index
    device_queue_create_info.queueCount = 4;            //check if this amount is available
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
}

int main() {
    vkEngine engine;

    return 0;
}
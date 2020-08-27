#include "vkEngine.h"

vkEngine::vkEngine() {
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = nullptr;
    app_info.pApplicationName = "GAME";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.apiVersion = VK_API_VERSION_1_2;

    InitInstance();
    
}

vkEngine::~vkEngine() {
    DeinitInstance();
}

bool vkEngine::InitInstance()
{
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = NULL;
    instance_create_info.pApplicationInfo = &app_info;
    instance_create_info.enabledLayerCount = 0;
    instance_create_info.ppEnabledLayerNames = nullptr;
    instance_create_info.enabledExtensionCount = 0;
    instance_create_info.ppEnabledExtensionNames = nullptr;

    verify(vkCreateInstance(&instance_create_info, nullptr, &instance));
}

void vkEngine::DeinitInstance()
{
    vkDestroyInstance(instance, nullptr);
}

int main() {
    vkEngine engine;

    return 0;
}
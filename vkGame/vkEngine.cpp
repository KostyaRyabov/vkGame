#include "vkEngine.h"

vkEngine::vkEngine() {
    Settings::load();

    init_step(InitInstance());
    init_step(InitDebugReport());
    init_step(InitDevice());
    init_step(window.Create());
    init_step(InitSurface());
    init_step(InitQueue());
    init_step(InitSwapchain());
    init_step(InitSwapchainImages());
    init_step(InitPipelines());
    init_step(InitFramebuffer());
    init_step(InitCommandPool());
    window.Open();
}

vkEngine::~vkEngine() {
    vkDeviceWaitIdle(device);

    deinit_step(DeinitCommandPool());
    deinit_step(DeinitFramebuffer());
    deinit_step(DeinitPipelines());
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

    std::vector<const char*> requierd_layer_list{
        "VK_LAYER_LUNARG_standard_validation",
        "VK_LAYER_KHRONOS_validation",
        "VK_LAYER_AMD_switchable_graphics",
        "VK_LAYER_LUNARG_monitor"
    };

    std::vector<const char*> requiered_extension_list{
        "VK_EXT_debug_report",
        "VK_KHR_surface",
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    {
        uint32_t count = 0;
        vk_assert(vkEnumerateInstanceLayerProperties(&count, nullptr));
        std::vector<VkLayerProperties> available_layer_list(count);
        vk_assert(vkEnumerateInstanceLayerProperties(&count, available_layer_list.data()));
        
        count = 0;
        vk_assert(vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
        std::vector<VkExtensionProperties> available_extension_list(count);
        vk_assert(vkEnumerateInstanceExtensionProperties(nullptr, &count, available_extension_list.data()));
        
        {
            std::cout << "layers :\n";
            for (auto &layer : available_layer_list) {
                std::cout << "\t" << layer.layerName << std::endl;
            }

            std::cout << "\nextansions :\n";
            for (auto &extension : available_extension_list) {
                std::cout << "\t" << extension.extensionName << std::endl;
            }
            std::cout << std::endl;
        }

        {
            bool founded;

            for (auto& required_layer : requierd_layer_list) {
                founded = false;

                for (auto& available_layer : available_layer_list) {
                    if (!strcmp(available_layer.layerName, required_layer)) {
                        founded = true;
                        break;
                    }
                }

                if (!founded) return false;
            }

            for (auto& required_extension : requiered_extension_list) {
                founded = false;

                for (auto& available_extension : available_extension_list) {
                    if (!strcmp(available_extension.extensionName, required_extension)) {
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
    instance_create_info.enabledLayerCount = requierd_layer_list.size();
    instance_create_info.ppEnabledLayerNames = requierd_layer_list.data();
    instance_create_info.enabledExtensionCount = requiered_extension_list.size();
    instance_create_info.ppEnabledExtensionNames = requiered_extension_list.data();
    
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
        //VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
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
    uint32_t count = 0;
    vk_assert(vkEnumeratePhysicalDevices(instance, &count, nullptr));
    std::vector<VkPhysicalDevice> physical_device_list(count);
    vk_assert(vkEnumeratePhysicalDevices(instance, &count, physical_device_list.data()));

    // show Physical Device list
    for (auto &physicalDevice : physical_device_list) {
        vkGetPhysicalDeviceProperties(physicalDevice, &physical_device_properties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &physical_device_features);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physical_device_memory_properties);

        std::cout << "deviceName : " << physical_device_properties.deviceName << std::endl;
        std::cout << "apiVersion : "
            << VK_VERSION_MAJOR(physical_device_properties.apiVersion) << "."
            << VK_VERSION_MINOR(physical_device_properties.apiVersion) << "."
            << VK_VERSION_PATCH(physical_device_properties.apiVersion) << std::endl << std::endl;
    }

    std::vector<const char*> required_extension_list = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    for (auto& physicalDevice : physical_device_list) {
        if (DeviceExtensionsSupport(physicalDevice, required_extension_list)) {
            if (GetQueueFamily(physicalDevice, VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT | VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, graphics_queue_family_index)) {
                vkGetPhysicalDeviceFeatures(physicalDevice, &physical_device_features);
                vkGetPhysicalDeviceProperties(physicalDevice, &physical_device_properties);
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physical_device_memory_properties);

                //поиск необходимых характеристик

                physical_device = physicalDevice;
            }
        }
    }

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
    device_create_info.enabledExtensionCount = required_extension_list.size();
    device_create_info.ppEnabledExtensionNames = required_extension_list.data();
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

        uint32_t count = 0;
        vk_assert(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, nullptr));
        supported_surface_format_list.resize(count);
        vk_assert(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &count, supported_surface_format_list.data()));

        count = 0;
        vk_assert(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &count, nullptr));
        supported_presentation_mode_list.resize(count);
        vk_assert(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &count, supported_presentation_mode_list.data()));
    }

    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = nullptr;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = 3;                   // (surface_capabilities.minImageCount)
    swapchain_create_info.imageFormat = supported_surface_format_list[0].format;                // TODO: later find optimal
    swapchain_create_info.imageColorSpace = supported_surface_format_list[0].colorSpace;        //
    swapchain_create_info.imageExtent = { Settings::Window::width, Settings::Window::height };
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = nullptr;
    swapchain_create_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = supported_presentation_mode_list[1];                    // TODO: later find optimal 
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    return vk_verify(vkCreateSwapchainKHR(device, &swapchain_create_info, nullptr, &swapchain));
}

void vkEngine::DeinitSwapchain()
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

bool vkEngine::InitSwapchainImages()
{
    amountOfImagesInSwapchain = 0;
    vk_assert(vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, nullptr));
    VkImage* swapchain_image_list = new VkImage[amountOfImagesInSwapchain];
    vk_assert(vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, swapchain_image_list));

    image_view_list = new VkImageView[amountOfImagesInSwapchain];

    for (uint32_t i = 0; i < amountOfImagesInSwapchain; i++) {
        VkImageViewCreateInfo imageView_create_info = {};
        imageView_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageView_create_info.pNext = nullptr;
        imageView_create_info.flags = 0;
        imageView_create_info.image = swapchain_image_list[i];
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

        vk_assert(vkCreateImageView(device, &imageView_create_info, nullptr, &image_view_list[i]));
    };
    
    return true;
}

void vkEngine::DeinitSwapchainImages()
{
    for (uint32_t i = 0; i < amountOfImagesInSwapchain; i++) {
        vkDestroyImageView(device, image_view_list[i], nullptr);
    };

    delete[] image_view_list;
}

bool vkEngine::InitPipelines()
{
    vk_check(CreateShaderModule("vert.spv", &shader_module_vert));
    vk_check(CreateShaderModule("frag.spv", &shader_module_frag));

    VkPipelineShaderStageCreateInfo shader_stage_create_info_vert;
    shader_stage_create_info_vert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_info_vert.pNext = nullptr;
    shader_stage_create_info_vert.flags = 0;
    shader_stage_create_info_vert.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stage_create_info_vert.module = shader_module_vert;
    shader_stage_create_info_vert.pName = "main";
    shader_stage_create_info_vert.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shader_stage_create_info_frag;
    shader_stage_create_info_frag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_create_info_frag.pNext = nullptr;
    shader_stage_create_info_frag.flags = 0;
    shader_stage_create_info_frag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stage_create_info_frag.module = shader_module_frag;
    shader_stage_create_info_frag.pName = "main";
    shader_stage_create_info_frag.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shader_stages_create_info[] = { shader_stage_create_info_vert, shader_stage_create_info_frag };

    VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {};
    vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state_create_info.pNext = nullptr;
    vertex_input_state_create_info.flags = 0;
    vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
    vertex_input_state_create_info.pVertexBindingDescriptions = nullptr;
    vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;
    vertex_input_state_create_info.pVertexAttributeDescriptions = nullptr;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {};
    input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_state_create_info.pNext = nullptr;
    input_assembly_state_create_info.flags = 0;
    input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = Settings::Window::width;
    viewport.height = Settings::Window::height;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    VkRect2D scissor;
    scissor.extent = { Settings::Window::width, Settings::Window::height };
    scissor.offset = { 0,0 };

    VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.pNext = nullptr;
    viewport_state_create_info.flags = 0;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.pViewports = &viewport;
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.pScissors = &scissor;
    
    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
    rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_create_info.pNext = nullptr;
    rasterization_state_create_info.flags = 0;
    rasterization_state_create_info.depthClampEnable = VK_FALSE;
    rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization_state_create_info.depthBiasEnable = VK_FALSE;
    rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_create_info.depthBiasClamp = 0.0f;
    rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;
    rasterization_state_create_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
    multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_create_info.pNext = nullptr;
    multisample_state_create_info.flags = 0;
    multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    multisample_state_create_info.minSampleShading = 1.0f;
    multisample_state_create_info.pSampleMask = nullptr;
    multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
    multisample_state_create_info.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {};
    color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_create_info.pNext = nullptr;
    color_blend_state_create_info.flags = 0;
    color_blend_state_create_info.logicOpEnable = VK_FALSE;
    color_blend_state_create_info.logicOp = VK_LOGIC_OP_NO_OP;
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
    color_blend_state_create_info.blendConstants[0] = 0.0f;
    color_blend_state_create_info.blendConstants[1] = 0.0f;
    color_blend_state_create_info.blendConstants[2] = 0.0f;
    color_blend_state_create_info.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = nullptr;
    pipeline_layout_create_info.flags = 0;
    pipeline_layout_create_info.setLayoutCount = 0;
    pipeline_layout_create_info.pSetLayouts = nullptr;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = nullptr;

    vk_assert(vkCreatePipelineLayout(device, &pipeline_layout_create_info, nullptr, &pipeline_layout));

    VkAttachmentDescription attachment_description = {};
    attachment_description.flags = 0;
    attachment_description.format = supported_surface_format_list[0].format;
    attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachment_reference = {};
    attachment_reference.attachment = 0;
    attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass_description = {};
    subpass_description.flags = 0;
    subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_description.inputAttachmentCount = 0;
    subpass_description.pInputAttachments = nullptr;
    subpass_description.colorAttachmentCount = 1;
    subpass_description.pColorAttachments = &attachment_reference;
    subpass_description.pResolveAttachments = nullptr;
    subpass_description.pDepthStencilAttachment = nullptr;
    subpass_description.preserveAttachmentCount = 0;
    subpass_description.pPreserveAttachments = nullptr;

    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpass_dependency.dependencyFlags = 0;

    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.pNext = nullptr;
    render_pass_create_info.flags = 0;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &attachment_description;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = 0;
    render_pass_create_info.pDependencies = nullptr;

    vk_assert(vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass));

    VkGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.pNext = nullptr;
    pipeline_create_info.flags = 0;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = shader_stages_create_info;
    pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
    pipeline_create_info.pTessellationState = nullptr;
    pipeline_create_info.pViewportState = &viewport_state_create_info;
    pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
    pipeline_create_info.pMultisampleState = &multisample_state_create_info;
    pipeline_create_info.pDepthStencilState = nullptr;
    pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
    pipeline_create_info.pDynamicState = nullptr;
    pipeline_create_info.layout = pipeline_layout;
    pipeline_create_info.renderPass = render_pass;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    return vk_verify(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline));
}

void vkEngine::DeinitPipelines()
{
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, render_pass, nullptr);
    vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    vkDestroyShaderModule(device, shader_module_vert, nullptr);
    vkDestroyShaderModule(device, shader_module_frag, nullptr);
}

bool vkEngine::InitFramebuffer()
{
    frame_buffer_list = new VkFramebuffer[amountOfImagesInSwapchain];

    for (uint32_t i = 0; i < amountOfImagesInSwapchain; i++) {
        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext = nullptr;
        framebuffer_create_info.flags = 0;
        framebuffer_create_info.renderPass = render_pass;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = &image_view_list[i];
        framebuffer_create_info.width = Settings::Window::width;
        framebuffer_create_info.height = Settings::Window::height;
        framebuffer_create_info.layers = 1;

        vk_assert(vkCreateFramebuffer(device, &framebuffer_create_info, nullptr, &frame_buffer_list[i]));
    };

    return true;
}

void vkEngine::DeinitFramebuffer()
{
    for (uint32_t i = 0; i < amountOfImagesInSwapchain; i++) {
        vkDestroyFramebuffer(device, frame_buffer_list[i], nullptr);
    };

    delete[] frame_buffer_list;
}

bool vkEngine::InitCommandPool() {
    if (!GetQueueFamily(physical_device, VK_QUEUE_COMPUTE_BIT, command_family_index)) return false;

    VkCommandPoolCreateInfo command_pool_create_info = {};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = nullptr;
    command_pool_create_info.flags = 0;
    command_pool_create_info.queueFamilyIndex = command_family_index;

    vk_assert(vkCreateCommandPool(device, &command_pool_create_info, nullptr, &command_pool));

    VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = nullptr;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = amountOfImagesInSwapchain;

    command_buffer_list = new VkCommandBuffer[amountOfImagesInSwapchain];
    vk_assert(vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffer_list));

    VkCommandBufferBeginInfo command_buffer_begin_info = {};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = nullptr;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    command_buffer_begin_info.pInheritanceInfo = nullptr;

    for (uint32_t i = 0; i < amountOfImagesInSwapchain; i++) {
        vk_assert(vkBeginCommandBuffer(command_buffer_list[i], &command_buffer_begin_info));

        VkClearValue clear_value = { 0.0f, 0.0f, 0.0f, 1.0f };

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = nullptr;
        render_pass_begin_info.renderPass = render_pass;
        render_pass_begin_info.framebuffer = frame_buffer_list[i];
        render_pass_begin_info.renderArea.offset = { 0, 0 };
        render_pass_begin_info.renderArea = { static_cast<int32_t>(Settings::Window::width),  static_cast<int32_t>(Settings::Window::height) };
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_value;

        vkCmdBeginRenderPass(command_buffer_list[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(command_buffer_list[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        vkCmdDraw(command_buffer_list[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(command_buffer_list[i]);

        vk_assert(vkEndCommandBuffer(command_buffer_list[i]));
    }

    VkSemaphoreCreateInfo semaphore_create_info = {};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_create_info.pNext = nullptr;
    semaphore_create_info.flags = 0;

    vk_assert(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore_image_available));
    vk_assert(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &semaphore_rendering_done));

    return true;
}

void vkEngine::DeinitCommandPool() {
    
    vkDestroySemaphore(device, semaphore_image_available, nullptr);
    vkDestroySemaphore(device, semaphore_rendering_done, nullptr);

    vkFreeCommandBuffers(device, command_pool, amountOfImagesInSwapchain, command_buffer_list);
    delete[] command_buffer_list;
    vkDestroyCommandPool(device, command_pool, nullptr);
}

//---------------

bool vkEngine::CreateShaderModule(const char* filename, VkShaderModule *shader_module)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file) return false;
    
    size_t fileSize = file.tellg();
    std::vector<char> shaderCode(fileSize);
    file.seekg(0);
    file.read(shaderCode.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo shader_module_create_info = {};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = nullptr;
    shader_module_create_info.flags = 0;
    shader_module_create_info.codeSize = shaderCode.size();
    shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    return vk_verify(vkCreateShaderModule(device, &shader_module_create_info, nullptr, shader_module));
}

bool vkEngine::DrawFrame() {
    uint32_t image_index;
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphore_image_available, VK_NULL_HANDLE, &image_index);
    
    VkPipelineStageFlags wait_stage_mask = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = nullptr;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &semaphore_image_available;
    submit_info.pWaitDstStageMask = &wait_stage_mask;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer_list[image_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &semaphore_rendering_done;

    vk_assert(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE));

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.pNext = nullptr;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &semaphore_rendering_done;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    return vk_verify(vkQueuePresentKHR(queue, &present_info));
}

bool vkEngine::DeviceExtensionsSupport(const VkPhysicalDevice& pysicalDevice, std::vector<const char*>& requiredExtensions)
{
    uint32_t count = 0;
    vk_assert(vkEnumerateDeviceExtensionProperties(pysicalDevice, nullptr, &count, nullptr));
    std::vector<VkExtensionProperties> available_extension_list(count);
    vk_assert(vkEnumerateDeviceExtensionProperties(pysicalDevice, nullptr, &count, available_extension_list.data()));

    bool founded;

    for (auto& required_extension : requiredExtensions) {
        founded = false;

        for (auto& available_extension : available_extension_list) {
            if (!strcmp(available_extension.extensionName, required_extension)) {
                founded = true;
                break;
            }
        }

        if (!founded) return false;
    };

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
        DrawFrame();
    }
}

int main() {
    vkEngine engine;
    engine.Run();
    return 0;
}
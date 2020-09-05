#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include "vulkan/vulkan.h"
#include "Window.h"
#include "Settings.h"
#include <fstream>
#include <iostream>
#include <vector>


namespace Settings {
    void load();
    void update();

    /*
    class Graphics {

    }

    class Processor {

    }
    */

    namespace Window {
        static unsigned _int32 width = 400;
        static unsigned _int32 height = 400;
    };
};

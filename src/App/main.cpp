#include <GLFW/glfw3.h>
#include <GlfwRuntime.hpp>
#include <LogMacros.hpp>
#include <PhysicalDeviceSelection.hpp>
#include <VulkanInstance.hpp>
#include <VulkanSurface.hpp>
#include <Window.hpp>
#include <cstdint>
#include <span>
#include <vector>
#include <VulkanDevice.hpp>

using namespace sya;

int main()
{
    try
    {

        LOGI("Sya started");
        {
            GlfwRuntime runtime;

            std::uint32_t extensionCount{};
            const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

            if (!extensions)
            {
                throw std::runtime_error{"GLFW could not provide Vulkan surface extensions"};
            }

            std::vector<const char*> instanceExtensions{extensions, extensions + extensionCount};

#ifndef NDEBUG
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

            VulkanInstance vulkan{instanceExtensions};

            Window window(1280, 720, "Sya");

            VulkanSurface surface{vulkan.nativeHandle(), window.nativeHandle()};

            const auto selection = selectPhysicalDevice(vulkan.nativeHandle(), surface.nativeHandle());

            VulkanDevice device{selection.device, selection.queueFamilyIndex};

            while (!window.shouldClose())
            {
                glfwWaitEvents();
            }
        }

        LOGI("Sya stopped");
        return 0;
    }
    catch (const std::exception& error)
    {
        LOGE("Application failed: ", error.what());
        return 1;
    }
}

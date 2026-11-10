#include <GLFW/glfw3.h>
#include <GlfwRuntime.hpp>
#include <LogMacros.hpp>
#include <Window.hpp>
#include <stdexcept>
#include <cstdint>
#include <VulkanInstance.hpp>
#include <span>

using namespace sya;

int main()
{
    try
    {

        LOGI("Sya started");
        {
            GlfwRuntime GRuntime;

            std::uint32_t extensionCount{};
            const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

            if (!extensions)
            {
                throw std::runtime_error{"GLFW could not provide Vulkan surface extensions"};
            }

            VulkanInstance vulkan{std::span<const char* const>{extensions, extensionCount}};

            Window window(1280, 720, "Sya");

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
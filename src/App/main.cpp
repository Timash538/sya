#include <GLFW/glfw3.h>
#include <GlfwRuntime.hpp>
#include <LogMacros.hpp>
#include <VulkanInstance.hpp>
#include <VulkanSurface.hpp>
#include <Window.hpp>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

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
            std::vector<VkPhysicalDevice> devices;
            VkPhysicalDevice chosenDevice{VK_NULL_HANDLE};
            uint32_t chosenFamilyIndex{};
            while (true)
            {
                std::uint32_t count{};
                VkResult result;

                result = vkEnumeratePhysicalDevices(vulkan.nativeHandle(), &count, nullptr);

                if (result != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to enumerate physical devices");
                }
                if (count == 0)
                {
                    throw std::runtime_error("No Vulkan devices found");
                }
                devices.resize(count);
                result = vkEnumeratePhysicalDevices(vulkan.nativeHandle(), &count, devices.data());

                if (result == VK_INCOMPLETE)
                {
                    continue;
                }
                if (result != VK_SUCCESS)
                {
                    throw std::runtime_error("Failed to enumerate physical devices");
                }
                devices.resize(count);
                break;
            }
            for (const auto& device : devices)
            {
                VkPhysicalDeviceProperties properties{};
                vkGetPhysicalDeviceProperties(device, &properties);
                LOGI(properties.deviceName);

                std::uint32_t familyCount{};
                vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

                std::vector<VkQueueFamilyProperties> families(familyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());
                for (uint32_t i = 0; i < familyCount; ++i)
                {

                    VkBool32 canPresent = VK_FALSE;
                    const VkResult result =
                        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface.nativeHandle(), &canPresent);

                    if (result != VK_SUCCESS)
                    {
                        throw std::runtime_error{"Failed to query presentation support"};
                    }

                    bool canDraw = (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;

                    if (canDraw && canPresent == VK_TRUE)
                    {
                        LOGI("Selected graphics and presentation queue family", i);
                        chosenDevice = device;
                        chosenFamilyIndex = i;
                        break;
                    }
                }
                if (chosenDevice != VK_NULL_HANDLE)
                    break;
            }
            if (chosenDevice == VK_NULL_HANDLE)
            {
                throw std::runtime_error("Failed to choose a device");
            }
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
#include <PhysicalDeviceSelection.hpp>

#include <LogMacros.hpp>
#include <stdexcept>
#include <vector>

namespace sya
{

PhysicalDeviceSelection selectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
    std::vector<VkPhysicalDevice> devices;
    while (true)
    {
        std::uint32_t count{};
        VkResult result;

        result = vkEnumeratePhysicalDevices(instance, &count, nullptr);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to enumerate physical devices");
        }
        if (count == 0)
        {
            throw std::runtime_error("No Vulkan devices found");
        }
        devices.resize(count);
        result = vkEnumeratePhysicalDevices(instance, &count, devices.data());

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
        for (std::uint32_t i = 0; i < familyCount; ++i)
        {

            VkBool32 canPresent = VK_FALSE;
            const VkResult result =
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &canPresent);

            if (result != VK_SUCCESS)
            {
                throw std::runtime_error{"Failed to query presentation support"};
            }

            const bool canDraw = (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;

            if (canDraw && canPresent == VK_TRUE)
            {
                LOGI("Selected graphics and presentation queue family", i);
                return {device, i};
            }
        }
    }
    throw std::runtime_error{"No Vulkan device has a queue family supporting graphics and presentation"};
}

} // namespace sya

#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>

namespace sya
{

struct PhysicalDeviceSelection
{
    VkPhysicalDevice device;
    std::uint32_t queueFamilyIndex;
};

[[nodiscard]] PhysicalDeviceSelection selectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

} // namespace sya

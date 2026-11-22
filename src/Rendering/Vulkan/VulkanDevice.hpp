#pragma once

#include <cstdint>

#include <vulkan/vulkan.h>

namespace sya
{
class VulkanDevice
{
public:
    explicit VulkanDevice(VkPhysicalDevice _device, std::uint32_t _queueFamilyIndex);
    ~VulkanDevice();

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

    VkDevice nativeHandle() const noexcept;

private:
    VkDevice device{VK_NULL_HANDLE};
    VkQueue queue{VK_NULL_HANDLE};
};
} // namespace sya
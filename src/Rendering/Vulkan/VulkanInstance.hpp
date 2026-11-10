#pragma once

#include <vulkan/vulkan.h>
#include <span>

namespace sya
{

class VulkanInstance
{
public:
    explicit VulkanInstance(std::span<const char* const> extensions);
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance& operator=(const VulkanInstance&) = delete;

    VkInstance nativeHandle() const noexcept;

private:
    VkInstance instance{VK_NULL_HANDLE};
};

} // namespace sya

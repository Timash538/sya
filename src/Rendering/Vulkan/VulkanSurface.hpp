#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace sya
{

class VulkanSurface
{
public:
    VulkanSurface(VkInstance instance, GLFWwindow* window);
    ~VulkanSurface();

    VulkanSurface(const VulkanSurface&) = delete;
    VulkanSurface& operator=(const VulkanSurface&) = delete;

    VkSurfaceKHR nativeHandle() const noexcept;

private:
    VkInstance instance;
    VkSurfaceKHR surface{VK_NULL_HANDLE};
};

} // namespace sya

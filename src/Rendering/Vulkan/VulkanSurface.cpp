#include <VulkanSurface.hpp>
#include <stdexcept>

using namespace sya;

VulkanSurface::VulkanSurface(VkInstance _instance, GLFWwindow* window) : instance(_instance)
{
    const VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create Vulkan surface"};
    }
}

VulkanSurface::~VulkanSurface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

VkSurfaceKHR VulkanSurface::nativeHandle() const noexcept
{
    return surface;
}
#include <VulkanDevice.hpp>

#include <stdexcept>

using namespace sya;

VulkanDevice::VulkanDevice(VkPhysicalDevice _device, std::uint32_t _queueFamilyIndex)
{
    const float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = _queueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;

    VkResult result = vkCreateDevice(_device, &deviceInfo, nullptr, &device);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan device");
    }

    vkGetDeviceQueue(device, _queueFamilyIndex, 0, &queue);
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, nullptr);
}

VkDevice VulkanDevice::nativeHandle() const noexcept
{
    return device;
}
#include <VulkanInstance.hpp>

#include <cstdint>
#include <stdexcept>

using namespace sya;

VulkanInstance::VulkanInstance(std::span<const char* const> extensions)
{
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create Vulkan instance"};
    }
}

VulkanInstance::~VulkanInstance() {
    vkDestroyInstance(instance, nullptr);
}

VkInstance VulkanInstance::nativeHandle() const noexcept
{
    return instance;
}
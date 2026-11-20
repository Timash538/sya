#include <VulkanInstance.hpp>

#include <LogMacros.hpp>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

using namespace sya;

#ifndef NDEBUG
namespace
{
bool isValidationLayerAvailable()
{
    std::uint32_t count{};
    VkResult result;
    std::vector<VkLayerProperties> layers;

    while (true)
    {
        result = vkEnumerateInstanceLayerProperties(&count, nullptr);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to enumerate instance layer properties");
        }
        if (count == 0)
        {
            return false;
        }
        layers.resize(count);

        result = vkEnumerateInstanceLayerProperties(&count, layers.data());

        if (result == VK_INCOMPLETE)
        {
            continue;
        }

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to enumerate instance layer properties and put it in vector");
        }
        layers.resize(count);
        break;
    }

    for (const auto& layer : layers)
    {
        if (std::string_view{layer.layerName} == "VK_LAYER_KHRONOS_validation")
            return true;
    }
    return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                  VkDebugUtilsMessageTypeFlagsEXT,
                                                  const VkDebugUtilsMessengerCallbackDataEXT* data,
                                                  void*) noexcept
{
    switch (severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        LOGI(data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOGW(data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOGE(data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        LOGT(data->pMessage);
        break;
    }
    return VK_FALSE;
}
} // namespace
#endif

VulkanInstance::VulkanInstance(std::span<const char* const> extensions)
{
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifndef NDEBUG
    const char* validationLayer = "VK_LAYER_KHRONOS_validation";

    if (!isValidationLayerAvailable())
    {
        throw std::runtime_error{"Vulkan validation layer is unavailable"};
    }

    createInfo.enabledLayerCount = 1;
    createInfo.ppEnabledLayerNames = &validationLayer;

    VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    debugInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    debugInfo.pfnUserCallback = &validationCallback;

    createInfo.pNext = &debugInfo;
#endif

    const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error{"Failed to create Vulkan instance"};
    }

#ifndef NDEBUG
    const auto createDebugMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

    destroyDebugMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

    if (!createDebugMessenger || !destroyDebugMessenger)
    {
        vkDestroyInstance(instance, nullptr);
        throw std::runtime_error{"Vulkan debug messenger functions are unavailable"};
    }

    const VkResult messengerResult = createDebugMessenger(instance, &debugInfo, nullptr, &debugMessenger);

    if (messengerResult != VK_SUCCESS)
    {
        vkDestroyInstance(instance, nullptr);
        throw std::runtime_error{"Failed to create Vulkan debug messenger"};
    }

#endif
}

VulkanInstance::~VulkanInstance()
{

#ifndef NDEBUG
    if (debugMessenger != VK_NULL_HANDLE)
    {
        destroyDebugMessenger(instance, debugMessenger, nullptr);
    }
#endif

    vkDestroyInstance(instance, nullptr);
}

VkInstance VulkanInstance::nativeHandle() const noexcept
{
    return instance;
}
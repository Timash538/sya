add_library(sya_vulkan STATIC
    VulkanInstance.cpp
)

target_include_directories(sya_vulkan
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sya_vulkan PUBLIC Vulkan::Vulkan)
add_library(sya_vulkan STATIC
    VulkanInstance.cpp
    VulkanSurface.cpp)

target_include_directories(sya_vulkan
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sya_vulkan 
    PUBLIC 
        Vulkan::Vulkan 
        glfw
    PRIVATE
        sya_logger
)


target_compile_definitions(sya_vulkan PUBLIC GLFW_INCLUDE_NONE)

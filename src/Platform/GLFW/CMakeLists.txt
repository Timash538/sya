add_library(sya_glfw STATIC
    GlfwRuntime.cpp
    Window.cpp
)

target_include_directories(sya_glfw
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sya_glfw PUBLIC glfw)
target_compile_definitions(sya_glfw PUBLIC GLFW_INCLUDE_NONE)
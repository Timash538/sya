add_executable(sya)

target_sources(sya PRIVATE main.cpp)

target_link_libraries(sya PRIVATE
    sya_logger
    glfw
)

target_compile_definitions(sya PRIVATE GLFW_INCLUDE_NONE)
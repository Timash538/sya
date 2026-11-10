add_executable(sya)

target_sources(sya PRIVATE main.cpp)

target_link_libraries(sya PRIVATE
    sya_logger
    sya_glfw
)

target_compile_definitions(sya PRIVATE GLFW_INCLUDE_NONE)
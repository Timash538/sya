add_executable(sya)

target_sources(sya PRIVATE main.cpp)

target_link_libraries(sya PRIVATE
    sya_spsc
)
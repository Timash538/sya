add_library(sya_spsc INTERFACE)

target_include_directories(sya_spsc
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sya_spsc
    INTERFACE
        sya_slot
)
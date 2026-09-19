add_library(sya_mpsc INTERFACE)

target_include_directories(sya_mpsc
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sya_mpsc
    INTERFACE
        sya_slot
        sya_hw
)
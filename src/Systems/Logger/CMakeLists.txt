add_library(sya_logger STATIC
    Logger.cpp
    LogFormatter.cpp
    SourceInfo.cpp
    ConsoleSink.cpp
    FileSink.cpp)

target_include_directories(sya_logger
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sya_logger
    PUBLIC sya_mpsc
)

find_package(Threads REQUIRED)

target_link_libraries(sya_logger
    PRIVATE Threads::Threads
)
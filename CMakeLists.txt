cmake_minimum_required(VERSION 3.20)

project(sya LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_SOURCE_DIR}/build
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG
    ${CMAKE_SOURCE_DIR}/build/Debug
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE
    ${CMAKE_SOURCE_DIR}/build/Release)

include(CTest)

add_subdirectory(src/Systems)
add_subdirectory(src/App)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
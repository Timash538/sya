cmake_minimum_required(VERSION 3.20)

project(sya LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/bin
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/lib
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR}/lib
)

include(CTest)

add_subdirectory(src/Core)
add_subdirectory(src/Systems)
add_subdirectory(src/App)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
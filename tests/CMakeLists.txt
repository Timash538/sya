include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(sya_tests
    SPSC/SPSCQueueTests.cpp
    Core/Memory/SlotTests.cpp
)

target_link_libraries(sya_tests
    PRIVATE
    sya_slot
    sya_spsc
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sya_tests)
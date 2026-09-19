include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(sya_tests
    Core/Memory/SlotTests.cpp
    SPSC/SPSCQueueTests.cpp
    MPSC/MPSCQueueTests.cpp
)

target_link_libraries(sya_tests
    PRIVATE
    sya_slot
    sya_spsc
    sya_mpsc
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(sya_tests)
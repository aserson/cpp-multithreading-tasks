cmake_minimum_required(VERSION 3.14)
project(cpp-multithreading-tests)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES "*.cpp")
add_executable(cpp-multithreading-tests ${TEST_SOURCES})

target_link_libraries(cpp-multithreading-tests gtest gtest_main)

enable_testing()
add_test(NAME all-tests COMMAND cpp-multithreading-tests)

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

add_executable(test-04 test-04.cpp)
add_executable(test-05 test-05.cpp)
add_executable(test-06 test-06.cpp)
add_executable(test-07 test-07.cpp)

target_link_libraries(test-04 gtest gtest_main)
target_link_libraries(test-05 gtest gtest_main)
target_link_libraries(test-06 gtest gtest_main)
target_link_libraries(test-07 gtest gtest_main)

enable_testing()
add_test(NAME test-04 COMMAND test-04)
add_test(NAME test-05 COMMAND test-05)
add_test(NAME test-06 COMMAND test-06)
add_test(NAME test-07 COMMAND test-07)
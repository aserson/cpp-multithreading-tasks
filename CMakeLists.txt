cmake_minimum_required(VERSION 3.14)
project(cpp-multithreading-tasks)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories("include")

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(cpp-multithreading-tasks ${SOURCES})

add_subdirectory(tests)

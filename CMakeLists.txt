cmake_minimum_required(VERSION 3.17)
project(cpp-multithreading-tasks)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

foreach(i RANGE 1 10)
    if(${i} LESS 10)
        set(idx "0${i}")
    else()
        set(idx "${i}")
    endif()

    set(PROJECT_NAME "test-${idx}")

    add_executable(${PROJECT_NAME} 
        ${CMAKE_SOURCE_DIR}/src/test-${idx}.cpp 
        ${CMAKE_SOURCE_DIR}/include/task-${idx}.h
    )
endforeach()
cmake_minimum_required(VERSION 3.25)
project(Flappy-Bird)

set(CMAKE_CXX_STANDARD 11)

find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

include_directories(src)

add_executable(flappy-bird src/main.cpp
        src/main.h)

target_link_libraries(flappy-bird ${SDL3_LIBRARIES})
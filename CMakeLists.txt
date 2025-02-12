cmake_minimum_required(VERSION 3.25)
project(Flappy-Bird)

set(CMAKE_CXX_STANDARD 11)

include_directories(src)

add_executable(flappy-bird src/main.cpp)

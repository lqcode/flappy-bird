cmake_minimum_required(VERSION 3.25)
project(Flappy-Bird)

set(CMAKE_CXX_STANDARD 11)

find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

find_package(Vulkan REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(Boost REQUIRED log)

include_directories(src)

add_executable(flappy-bird src/main.cpp
        src/main.h
        src/game/Game.cpp
        src/game/Game.h)

target_link_libraries(flappy-bird ${SDL3_LIBRARIES} glm::glm Vulkan::Vulkan Boost::log)
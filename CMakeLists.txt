cmake_minimum_required(VERSION 3.21)

project(CANetworkDesigner VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# FIXME: Remove this
target_compile_options(CANetworkDesigner PRIVATE -Wall -Werror)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(CANetworkDesigner main.cpp)

target_link_libraries(CANetworkDesigner PRIVATE 
    SDL2::SDL2 
    OpenGL::GL
)

# Prevent old compiler warnings
target_include_directories(CANetworkDesigner SYSTEM PRIVATE)

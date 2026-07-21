cmake_minimum_required(VERSION 3.21)

project(CANetworkDesigner VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(CANetworkDesigner
	main.cpp
	external/imgui/imgui.cpp
	external/imgui/imgui_draw.cpp
	external/imgui/imgui_tables.cpp
	external/imgui/imgui_widgets.cpp
	external/imgui/backends/imgui_impl_sdl2.cpp
	external/imgui/backends/imgui_impl_opengl2.cpp
)

# FIXME: Remove this
target_compile_options(CANetworkDesigner PRIVATE -Wall -Werror)

target_link_libraries(CANetworkDesigner PRIVATE 
    SDL2::SDL2 
    OpenGL::GL
)

# Prevent old compiler warnings
target_include_directories(CANetworkDesigner SYSTEM PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
	${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends)

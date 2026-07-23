cmake_minimum_required(VERSION 3.16)

project(CANetworkDesigner VERSION 0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

set(IMGUI_SOURCES
	external/imgui/imgui.cpp
	external/imgui/imgui_draw.cpp
	external/imgui/imgui_tables.cpp
	external/imgui/imgui_widgets.cpp
	external/imgui/backends/imgui_impl_sdl3.cpp
	external/imgui/backends/imgui_impl_opengl3.cpp
)

set(GLAD_SOURCES
	external/glad/glad.c
)

add_executable(CANetworkDesigner
	src/main.cpp
	${GLAD_SOURCES}
	${IMGUI_SOURCES}
)


# FIXME: Remove this
target_compile_options(CANetworkDesigner PRIVATE -Wall -Werror)

target_link_libraries(CANetworkDesigner PRIVATE 
	SDL3::SDL3 
	OpenGL::GL
	${CMAKE_DL_LIBS}
)

# Prevent old compiler warnings
target_include_directories(CANetworkDesigner SYSTEM PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann
	${CMAKE_CURRENT_SOURCE_DIR}/external/glad
	${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
	${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)


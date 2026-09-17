cmake_minimum_required(VERSION 4.2)

project(CANetworkDesigner VERSION 0.1 LANGUAGES C CXX)  # :)
set(CMAKE_CXX_STANDARD 17)                              # A bit on the modern side but whatever
set(CMAKE_CXX_STANDARD_REQUIRED ON)                     # Sure, I'll enforce the standard why not
set(CMAKE_CXX_EXTENSIONS OFF)                           # Don't need none of dat
# Build the release binary unless otherwise specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type" FORCE)
endif()

# Check if link time optimizations are supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error_output)

# Find required non-bundled dependencies
find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

# Source targets from imgui git submodule
set(IMGUI_SOURCES
        external/sqlite3/sqlite3.c
        external/imgui/imgui.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_sdl3.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        src/StateMachine.hpp
)

# Main executable
add_executable(CANetworkDesigner
        src/main.cpp
        external/glad/glad.c
        external/tinyfiledialogs/tinyfiledialogs.c
        ${IMGUI_SOURCES}
        src/GUI/GUI.hpp
        src/GUI/GUI.cpp
        src/GUI/Window/window.hpp
        src/GUI/Window/window.cpp
        src/GUI/OpenGL/OpenGL.hpp
        src/GUI/OpenGL/OpenGL.cpp
        src/Database/Project.hpp
        src/Database/Project.cpp
)

# Global definitions
target_compile_definitions(CANetworkDesigner PRIVATE
        IMGUI_DISABLE_DEFAULT_FONT  # We're using Jetbrains Mono instead
        SDL_MAIN_USE_CALLBACKS=1    # Use SDL callbacks instead of the typical main
        SDL_INCLUDE_NONE            # Prevents the OpenGL clash with glad
        # Enables sqlite features needed for undo functionality
        SQLITE_ENABLE_SESSION
        SQLITE_ENABLE_PREUPDATE_HOOK
)

# Enable link time optimizations
if(ipo_supported)
    set_target_properties(CANetworkDesigner PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Helps prevent me from being lazy
set_target_properties(CANetworkDesigner PROPERTIES
    COMPILE_WARNING_AS_ERROR ON
)

# System library linking. Good luck!
target_link_libraries(CANetworkDesigner PRIVATE
        SDL3::SDL3
        OpenGL::GL
)

# Add include directories for external dependencies
target_include_directories(CANetworkDesigner SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/external/fonts
        ${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite3
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyfiledialogs
)

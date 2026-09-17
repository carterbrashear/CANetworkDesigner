#include "window.hpp"

#include <stdexcept>

Window::Window() {
    // SDL OpenGL Attributes
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Window Creation
    window = SDL_CreateWindow(
        title,
        initial_width, initial_height,
        flags
    );
    // Throw an unrecoverable error on a fail
    if (!window) {
        SDL_Log("Window Error: %s", SDL_GetError());
        throw std::runtime_error("Window creation error!");
    }

    // Set Minimum Window Size
    SDL_SetWindowMinimumSize(window, min_width, min_height);

    // OpenGL Context Creation
    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        SDL_Log("OpenGL Error: %s", SDL_GetError());
        throw std::runtime_error("OpenGL context creation error!");
    }
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(static_cast<int>(vsync_state));
}

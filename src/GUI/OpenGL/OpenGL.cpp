#include "OpenGL.hpp"

#include <stdexcept>
#include <SDL3/SDL_video.h>

OpenGL::OpenGL() {
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
        throw std::runtime_error("Failed to initialize OpenGL!");
    }
}

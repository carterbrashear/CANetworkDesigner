#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <SDL3/SDL.h>

class Window {
    static constexpr char title[] = "CANetwork Designer";

    static constexpr int initial_width = 1280;
    static constexpr int initial_height = 720;
    static constexpr int min_width = 500;
    static constexpr int min_height = 500;

    static constexpr SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    enum class vsync : int {
        on=1,
        off=0,
        adaptive=-1
    };
    static constexpr auto vsync_state = vsync::on;

    SDL_Window* window;
    SDL_GLContext gl_context;
public:
    Window();

    SDL_Window* get() const { return window; }
    SDL_GLContext getGlContext() const { return gl_context; }

    ~Window() = default;
};

#endif // WINDOW_HPP

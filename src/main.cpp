#define SDL_MAIN_USE_CALLBACKS 1
#include <glad.h> // Ensure glad is included BEFORE SDL OpenGL headers
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "GUI/Window/window.hpp"
#include "GUI/OpenGL/OpenGL.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <jetbrains_mono.h>

// Application state structure passed between SDL callbacks
struct AppState {
    Window window;
    OpenGL open_gl;
    bool show_demo_window = true;
};

// ============================================================================
// 1. INITIALIZATION
// ============================================================================
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    auto* state = new AppState();
    *appstate = state;

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
    // Set font to JetBrains Mono
    ImFont* jetBrainsMonoFont = io.Fonts->AddFontFromMemoryCompressedTTF(
        JetBrainsMono_compressed_data,
        JetBrainsMono_compressed_size,
        20.0f
    );

    // Styling
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark(); // Default to dark mode
    // Add some nice rounded corners
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.ChildRounding = 5.0f;
    style.PopupRounding = 5.0f;

    // Setup ImGui Platform/Renderer Backends
    ImGui_ImplSDL3_InitForOpenGL(state->window.get(), state->window.getGlContext());
    ImGui_ImplOpenGL3_Init("#version 330");

    return SDL_APP_CONTINUE;
}

// ============================================================================
// 2. EVENT HANDLING
// ============================================================================
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    const auto* state = static_cast<AppState*>(appstate);

    // Forward events to ImGui
    ImGui_ImplSDL3_ProcessEvent(event);

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
        event->window.windowID == SDL_GetWindowID(state->window.get())) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

/**
 *
 * @param appstate
 * @return
 */
SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* state = static_cast<AppState*>(appstate);

    // Save window size
    int w = 0, h = 0;
    SDL_GetWindowSizeInPixels(state->window.get(), &w, &h);

    // Skip rendering if window size is 0 or negative
    if (w <= 0 || h <= 0) {
        return SDL_APP_CONTINUE;
    }

    // Start ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();

    // Render ImGui Demo Window
    if (state->show_demo_window) {
        ImGui::ShowDemoWindow(&state->show_demo_window);
    }

    // Render Custom Window
    {
        ImGui::Begin("CANetwork Designer");
        ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        ImGui::Text("Render Target Size: %d x %d", w, h);
        ImGui::End();
    }

    // Render Frame
    ImGui::Render();

    // Set viewport dynamically on resize
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    SDL_GL_SwapWindow(state->window.get());

    return SDL_APP_CONTINUE;
}

// ============================================================================
// 4. CLEANUP
// ============================================================================
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (appstate) {
        const auto* state = static_cast<AppState*>(appstate);

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        if (state->window.getGlContext()) {
            SDL_GL_DestroyContext(state->window.getGlContext());
        }
        if (state->window.get()) {
            SDL_DestroyWindow(state->window.get());
        }
        delete state;
    }
}

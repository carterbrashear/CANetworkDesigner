#include "GUI.hpp"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <jetbrains_mono.h>

/**
 * @brief Initializes the GUI with default parameters. \f$ \frac{1}{x} \f$
 */
GUI::GUI(const Window& win) : window(win) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Enable keyboard navigation and window docking
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;

    setFont();
    setDarkMode(true);
}

/**
 * @brief Sets the font of ImGui to JetBrains Mono.
 * Compressed ttf is stored in external/fonts/jetbrains_mono.h.
 */
void GUI::setFont() {
    const ImGuiIO& io = ImGui::GetIO();

    // Default to JetBrains Mono
    io.Fonts->AddFontFromMemoryCompressedTTF(
        JetBrainsMono_compressed_data,
        JetBrainsMono_compressed_size,
        18.0f
    );
}

/**
 * @brief Enables dark mode and light mode.
 * @param dark True to enable dark mode and false to enable light mode.
 */
void GUI::setDarkMode(const bool dark) {
    if (dark)
        ImGui::StyleColorsDark();
    else
        ImGui::StyleColorsLight();
}

void GUI::setStyles() {
    ImGuiStyle& style = ImGui::GetStyle();
    // Round all of our corners
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.ChildRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ImageRounding = 6.0f;
}

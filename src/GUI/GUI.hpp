/**
 * @file GUI.hpp
 * @section DESCRIPTION
 * Dumbed down ImGUI wrapper that contains all the elements needed.
 */

#ifndef CANETWORKDESIGNER_GUI_HPP
#define CANETWORKDESIGNER_GUI_HPP
#include "Window/window.hpp"

class GUI {
    Window window;
    static void setFont();
    static void setDarkMode(bool dark);

    static void setStyles();
public:
    explicit GUI(const Window& win);

    ~GUI() = default;
};


#endif //CANETWORKDESIGNER_GUI_HPP

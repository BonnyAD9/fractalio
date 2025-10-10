#pragma once

#include <memory>

#include "font.hpp"
#include "fractal.hpp"
#include "glfw/window.hpp"
#include "text_renderer/text_renderer.hpp"

namespace fio {

class Fractalio {
public:
    Fractalio(Fractalio &&) = delete;
    Fractalio &operator=(Fractalio &&) = delete;

    Fractalio(
        std::unique_ptr<glfw::Window> window, const char *font = "monospace"
    );

    void mainloop();

private:
    std::unique_ptr<glfw::Window> _window;
    std::unique_ptr<Fractal> _active;
    glm::dvec2 _last_mouse_pos;
    Font _font;
    TextRenderer _text_renderer;
    TextRenderer _fps_text;
    bool _new_text = true;
    float _width = 0;

    void size_callback(int width, int height);
    void mouse_move_callback(double x, double y);
    void scroll_callback(double dx, double dy);
    void process_input();
};

} // namespace fio
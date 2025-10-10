#pragma once

#include <memory>

#include "fractal.hpp"
#include "glfw/window.hpp"

namespace fio {

class Fractalio {
public:
    Fractalio(Fractalio &&) = delete;
    Fractalio &operator=(Fractalio &&) = delete;

    Fractalio(std::unique_ptr<glfw::Window> window);

    void mainloop();

private:
    std::unique_ptr<glfw::Window> _window;
    std::unique_ptr<Fractal> _active;
    glm::dvec2 _last_mouse_pos;

    void size_callback(int width, int height);
    void mouse_move_callback(double x, double y);
    void scroll_callback(double dx, double dy);
    void process_input();
};

} // namespace fio
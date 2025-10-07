#pragma once

#include <memory>
#include <optional>

#include "glfw/window.hpp"
#include "mandelbrot/mandelbrot.hpp"
namespace fio {

class Fractalio {
public:
    Fractalio(Fractalio &&) = delete;
    Fractalio &operator=(Fractalio &&) = delete;

    Fractalio(std::unique_ptr<glfw::Window> window);

    void mainloop();

private:
    std::unique_ptr<glfw::Window> _window;
    std::optional<Mandelbrot> _active;
    glm::dvec2 _last_mouse_pos;

    void size_callback(int width, int height);
    void mouse_move_callback(double x, double y);
    void process_input();
};

} // namespace fio
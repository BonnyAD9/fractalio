#pragma once

#include <memory>

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

    static void size_callback(int width, int height);
    void process_input();
};

} // namespace fio
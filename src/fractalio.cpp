#include "fractalio.hpp"

#include <memory>

#include "gl/gl.hpp"

namespace fio {

constexpr glm::vec4 DEFAULT_CLEAR_COLOR{ 0, 0, 0, 1 };

Fractalio::Fractalio(std::unique_ptr<glfw::Window> window) :
    _window(std::move(window)) {
    _window->make_context_current();
    auto size = _window->get_size();
    glViewport(0, 0, size.x, size.y);
    gl::clear_color(DEFAULT_CLEAR_COLOR);
    _window->set_size_callback([&](int w, int h) { size_callback(w, h); });

    _active.emplace();
    _active->resize(size);
}

void Fractalio::mainloop() {
    _window->make_context_current();
    if (_active) {
        _active->use();
    }

    while (!_window->should_close()) {
        process_input();

        glClear(GL_COLOR_BUFFER_BIT);
        if (_active) {
            _active->draw();
        }

        _window->swap_buffers();
        glfwPollEvents();
    }
}

void Fractalio::size_callback(int width, int height) {
    glViewport(0, 0, width, height);
    if (_active) {
        _active->use();
        _active->resize({ width, height });
    }
}

void Fractalio::process_input() { }

} // namespace fio
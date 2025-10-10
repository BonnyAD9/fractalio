#include "fractalio.hpp"

#include <memory>

#include "draw_tex/draw_tex.hpp"
#include "font.hpp"
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
    _window->set_mouse_move_callback([&](double x, double y) {
        mouse_move_callback(x, y);
    });
    _window->set_scroll_callback([&](double dx, double dy) {
        scroll_callback(dx, dy);
    });

    Font font("monospace", 32);
    _active = std::make_unique<DrawTex>(
        reinterpret_cast<const char *>(font.data()),
        GLsizei(font.width()),
        GLsizei(font.height()),
        GL_RED
    );
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

void Fractalio::mouse_move_callback(double x, double y) {
    const glm::dvec2 mouse_pos{ x, y };

    if (!_active) {
        _last_mouse_pos = mouse_pos;
        return;
    }

    if (glfwGetMouseButton(_window->get(), GLFW_MOUSE_BUTTON_LEFT) ==
        GLFW_PRESS) {
        auto delta = mouse_pos - _last_mouse_pos;
        _active->drag(delta);
    } else if (glfwGetMouseButton(_window->get(), GLFW_MOUSE_BUTTON_RIGHT) ==
               GLFW_PRESS) {
        _active->scale(mouse_pos.y - _last_mouse_pos.y);
    }

    _last_mouse_pos = mouse_pos;
}

void Fractalio::scroll_callback(double, double dy) {
    if (!_active) {
        return;
    }

    _active->scale(dy * 8);
}

void Fractalio::process_input() { }

} // namespace fio
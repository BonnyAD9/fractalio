#include "fractalio.hpp"

#include <format>
#include <memory>

#include "font.hpp"
#include "gl/gl.hpp"
#include "mandelbrot/mandelbrot.hpp"

namespace fio {

static constexpr std::size_t SIDE_WIDTH = 300;
static constexpr std::size_t FONT_SIZE = 16;

constexpr glm::vec4 DEFAULT_CLEAR_COLOR{ 0.1, 0.1, 0.1, 1 };

Fractalio::Fractalio(std::unique_ptr<glfw::Window> window, const char *font) :
    _window(std::move(window)),
    _font(font, FONT_SIZE),
    _text_renderer(_font, std::size_t(FONT_SIZE * 3 / 2)),
    _fps_text(_font, 0) {

    _window->make_context_current();
    glfwSwapInterval(0); // to disable vsync
    auto size = _window->get_size();
    _width = float(size.x);
    glViewport(0, 0, size.x, size.y);
    gl::clear_color(DEFAULT_CLEAR_COLOR);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    _window->set_size_callback([&](int w, int h) { size_callback(w, h); });
    _window->set_mouse_move_callback([&](double x, double y) {
        mouse_move_callback(x, y);
    });
    _window->set_scroll_callback([&](double dx, double dy) {
        scroll_callback(dx, dy);
    });

    _active = std::make_unique<Mandelbrot>();
    _active->resize({ 0, 0 }, { size.x - SIDE_WIDTH, size.y }, size);

    _text_renderer.use();
    _text_renderer.resize(size);

    _fps_text.use();
    _fps_text.resize(size);
}

void Fractalio::mainloop() {
    _window->make_context_current();

    double last_time = glfwGetTime();
    while (!_window->should_close()) {
        const double time = glfwGetTime();
        const double delta_time = time - last_time;
        last_time = time;

        process_input();

        if (_new_text) {
            _text_renderer.clear_text();
            if (_active) {
                auto text = _active->describe();
                _text_renderer.add_text(
                    text, { _width - SIDE_WIDTH + 10, FONT_SIZE + 10 }
                );
            }
            _text_renderer.use();
            _text_renderer.prepare();
            _new_text = false;
        }

        _fps_text.clear_text();
        auto fps = std::format("FPS: {}", std::size_t(1 / delta_time));
        _fps_text.add_text(fps, { _width - SIDE_WIDTH + 10, FONT_SIZE + 10 });
        _fps_text.use();
        _fps_text.prepare();

        glClear(GL_COLOR_BUFFER_BIT);
        if (_active) {
            _active->use();
            _active->draw();
        }

        _text_renderer.use();
        _text_renderer.draw();

        _fps_text.use();
        _fps_text.draw();

        _window->swap_buffers();
        glfwPollEvents();
    }
}

void Fractalio::size_callback(int width, int height) {
    glViewport(0, 0, width, height);
    _new_text = true;
    _width = float(width);
    if (_active) {
        _active->use();
        _active->resize(
            { 0, 0 }, { width - SIDE_WIDTH, height }, { width, height }
        );
    }

    _text_renderer.use();
    _text_renderer.resize({ width, height });

    _fps_text.use();
    _fps_text.resize({ width, height });
}

void Fractalio::mouse_move_callback(double x, double y) {
    const glm::dvec2 mouse_pos{ x, y };

    if (!_active) {
        _last_mouse_pos = mouse_pos;
        return;
    }

    _active->use();

    if (glfwGetMouseButton(_window->get(), GLFW_MOUSE_BUTTON_LEFT) ==
        GLFW_PRESS) {
        auto delta = mouse_pos - _last_mouse_pos;
        _active->drag(delta);
        _new_text = true;
    } else if (glfwGetMouseButton(_window->get(), GLFW_MOUSE_BUTTON_RIGHT) ==
               GLFW_PRESS) {
        _active->scale(mouse_pos.y - _last_mouse_pos.y);
        _new_text = true;
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
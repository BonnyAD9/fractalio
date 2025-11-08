#include "fractalio.hpp"

#include <format>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <print>
#include <utility>

#include <GLFW/glfw3.h>

#include "burning_ship/burning_ship.hpp"
#include "commander.hpp"
#include "font.hpp"
#include "gl/gl.hpp"
#include "help.hpp"
#include "julia/julia.hpp"
#include "mandelbrot/mandelbrot.hpp"
#include "maps.hpp"
#include "powerbrot/powerbrot.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace fio {

constexpr double FPS_INTERVAL = 0.1;

Fractalio::Fractalio(std::unique_ptr<glfw::Window> window, const char *font) :
    _window((window->make_context_current(), std::move(window))),
    _font(font, FONT_SIZE),
    _info(_font, std::size_t(FONT_SIZE * 5 / 4)),
    _fps_text(_info),
    _commander(*this, _info) {

    auto size = _window->get_size();
    _wsize = size;
    glfwSwapInterval(1);

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
    _window->set_char_callback([&](unsigned c) { char_callback(c); });
    _window->set_key_callback([&](int k, int s, int a, int m) {
        key_callback(k, s, a, m);
    });
    _window->set_mouse_press_callback([&](int b, int a, int m) {
        mouse_press_callback(b, a, m);
    });

    init_fractals();

    _active->use();
    _active->resize({ 0, 0 }, { size.x - SIDE_WIDTH, size.y }, size);
    auto picker = _active->picker();
    if (picker) {
        picker->use();
        picker->resize(
            { _wsize.x - SIDE_WIDTH, _wsize.y - FONT_SIZE - SIDE_WIDTH },
            { SIDE_WIDTH, SIDE_WIDTH },
            _wsize
        );
    }

    const glm::vec2 side_start{ _wsize.x - SIDE_WIDTH + 10, 20 };

    _info.use();
    _info.resize(size);
    _info.set_transform(
        glm::translate(glm::identity<glm::mat4>(), { side_start, 0 })
    );

    _fps_text.use();
    _fps_text.resize(size);
    _fps_text.set_transform(
        glm::translate(glm::identity<glm::mat4>(), { side_start, 0 })
    );

    _commander.resize(size);
}

void Fractalio::mainloop() {
    _window->make_context_current();

    double last_time = glfwGetTime();
    double next_fps = 0;
    while (!_window->should_close()) {
        const double time = glfwGetTime();
        const double delta_time = time - last_time;
        last_time = time;

        process_input();

        if (_new_info) {
            _info.clear_text();
            if (_active) {
                auto text = _focus->describe();
                _info.add_text(text, { 0, 0 });
            }
            _info.use();
            _info.prepare();
            _new_info = false;
        }

        if (last_time > next_fps) {
            _fps_text.clear_text();
            auto fps = std::format("FPS: {}", std::size_t(1 / delta_time));
            _fps_text.add_text(fps, { 0, 0 });
            _fps_text.use();
            _fps_text.prepare();
            next_fps = last_time + FPS_INTERVAL;
        }

        _commander.prepare();

        glClear(GL_COLOR_BUFFER_BIT);
        _active->use();
        _active->draw();
        auto picker = _active->picker();
        if (picker) {
            picker->use();
            picker->draw();
        }

        _info.use();
        _info.draw();

        _fps_text.use();
        _fps_text.draw();

        _commander.draw();

        _window->swap_buffers();
        glfwPollEvents();
    }
}

void Fractalio::init_fractals() {
    _fractals[Fractal::Type::HELP] = std::make_unique<Help>(_info);
    _fractals[Fractal::Type::MANDELBROT] = std::make_unique<Mandelbrot>();
    _fractals[Fractal::Type::JULIA] = std::make_unique<Julia>();
    _fractals[Fractal::Type::BURNING_SHIP] = std::make_unique<BurningShip>();
    _fractals[Fractal::Type::POWERBROT] = std::make_unique<Powerbrot>();

    _active = _fractals[Fractal::Type::MANDELBROT].get();
    _focus = _active;
}

void Fractalio::size_callback(int width, int height) {
    glViewport(0, 0, width, height);
    _wsize = { width, height };

    _active->use();
    _active->resize({ 0, 0 }, { width - SIDE_WIDTH, height }, _wsize);
    auto picker = _active->picker();
    if (picker) {
        picker->use();
        picker->resize(
            { _wsize.x - SIDE_WIDTH, _wsize.y - FONT_SIZE * 1.5 - SIDE_WIDTH },
            { SIDE_WIDTH, SIDE_WIDTH },
            _wsize
        );
    }

    _info.use();
    _info.resize(_wsize);
    _info.set_transform(
        glm::translate(
            glm::identity<glm::mat4>(), { _wsize.x - SIDE_WIDTH + 10, 20, 0 }
        )
    );

    _fps_text.use();
    _fps_text.resize(_wsize);
    _fps_text.set_transform(
        glm::translate(
            glm::identity<glm::mat4>(), { _wsize.x - SIDE_WIDTH + 10, 20, 0 }
        )
    );

    _commander.resize(_wsize);
}

void Fractalio::mouse_move_callback(double x, double y) {
    const glm::dvec2 mouse_pos{ x, y };

    if (!_drag) {
        _last_mouse_pos = mouse_pos;
        return;
    }

    _drag->drag(mouse_pos, mouse_pos - _last_mouse_pos);
    _new_info = true;
    _last_mouse_pos = mouse_pos;
}

void Fractalio::mouse_press_callback(int button, int action, int mods) {
    if (_drag) {
        if (action == GLFW_RELEASE) {
            _drag->drag_end(button);
            return;
        }
        _drag->drag_end(std::nullopt);
    }

    auto bot_size = _wsize.y - FONT_SIZE * 1.5;

    if (_last_mouse_pos.x < _wsize.x - SIDE_WIDTH) {
        _drag = _active;
    } else if (_active && _last_mouse_pos.y > bot_size - SIDE_WIDTH &&
               _last_mouse_pos.y < bot_size) {
        if (_active) {
            _drag = _active->picker();
        }
    }

    if (_drag) {
        _drag->drag_start(button, mods, _last_mouse_pos);
    }
}

void Fractalio::scroll_callback(double, double dy) {
    if (!_active) {
        return;
    }

    _active->use();
    _active->scale(dy * 8);
}

void Fractalio::key_callback(int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
    case GLFW_KEY_ESCAPE:
        _commander.cancel();
        break;
    case GLFW_KEY_ENTER:
        _commander.consume();
        break;
    case GLFW_KEY_BACKSPACE:
        _commander.backspace();
        break;
    case GLFW_KEY_TAB:
        if (_active) {
            _active->map_use_double(maps::negate);
            _new_info = true;
        }
    default:
        break;
    }
}

void Fractalio::char_callback(unsigned code) {
    if (code > std::numeric_limits<char>::max()) {
        return;
    }
    _commander.char_in(char(code));
}

void Fractalio::process_input() { }

void Fractalio::activate(Fractal::Type typ) {
    if (!_fractals.contains(typ)) {
        std::println(std::cerr, "Unknown fractal type.");
        return;
    }
    _active = _fractals[typ].get();
    _focus = _active;
    _active->use();
    _active->resize({ 0, 0 }, { _wsize.x - SIDE_WIDTH, _wsize.y }, _wsize);
    _new_info = true;
    auto picker = _active->picker();
    if (picker) {
        picker->use();
        picker->resize(
            { _wsize.x - SIDE_WIDTH, _wsize.y - FONT_SIZE * 1.5 - SIDE_WIDTH },
            { SIDE_WIDTH, SIDE_WIDTH },
            _wsize
        );
    }
}

} // namespace fio
#include "fractalio.hpp"

#include <cmath>
#include <format>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <print>
#include <thread>
#include <utility>

#include <GLFW/glfw3.h>

#include "../fractals/burning_julia.hpp"
#include "../fractals/burning_ship.hpp"
#include "../fractals/double_pendulum.hpp"
#include "../fractals/help.hpp"
#include "../fractals/julia.hpp"
#include "../fractals/mandelbrot.hpp"
#include "../fractals/newton.hpp"
#include "../fractals/powerbrot.hpp"
#include "../fractals/three_body.hpp"
#include "../ft/font.hpp"
#include "../gl/gl.hpp"
#include "../gradient.hpp"
#include "commander.hpp"
#include "maps.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace fio::app {

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

    _gradient_1d.bind(GL_TEXTURE_1D);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::vector<glm::u8vec3> grad(256);
    gradient::ultra_fractal(grad);
    gl::tex_image_1d(grad);

    init_fractals();

    _active->resize(size);
    auto picker = _active->picker();
    if (picker) {
        picker->resize(_wsize);
    }

    const glm::vec2 side_start{ _wsize.x - SIDE_WIDTH + 10, 20 };

    _info.resize(size);
    _info.set_transform(
        glm::translate(glm::identity<glm::mat4>(), { side_start, 0 })
    );

    _fps_text.resize(size);
    _fps_text.set_transform(
        glm::translate(glm::identity<glm::mat4>(), { side_start, 0 })
    );
    
    _drag = _active;

    _commander.resize(size);
}

void Fractalio::mainloop() {
    _window->make_context_current();

    double last_time = glfwGetTime();
    double last_fps = glfwGetTime();
    double next_fps = 0;
    int fcnt = 0;
    while (!_window->should_close()) {
        double time = glfwGetTime();
        double delta = time - last_time;
        if (delta < _min_interval) {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(_min_interval - delta)
            );
            time = glfwGetTime();
            delta = time - last_time;
        }
        last_time = time;

        process_input();

        glClear(GL_COLOR_BUFFER_BIT);
        _active->draw(delta);
        auto picker = _active->picker();
        if (picker) {
            picker->draw(delta);
        }

        if (_new_info) {
            _info.clear_text();
            if (_active) {
                auto text = _focus->describe();
                _info.add_text(text, { 0, 0 });
            }
            _new_info = false;
        }
        _info.draw();

        if (time > next_fps) {
            const double delta_time = time - last_fps;
            last_fps = time;
            _fps_text.clear_text();
            auto fps = std::format(
                "FPS: {}", std::size_t(std::round(fcnt / delta_time))
            );
            _fps_text.add_text(fps, { 0, 0 });
            next_fps = last_fps + FPS_INTERVAL;
            fcnt = 0;
        }
        fcnt += 1;
        _fps_text.draw();

        _commander.draw();

        _window->swap_buffers();
        glfwPollEvents();
    }
}

void Fractalio::init_fractals() {
    const std::function<glm::mat3x2(glm::vec2)> s_fun =
        [=](glm::vec2 size) -> glm::mat3x2 {
        return { { 0, 0 }, { size.x - SIDE_WIDTH, size.y }, size };
    };
    const std::function<glm::mat3x2(glm::vec2)> sp_fun =
        [=](glm::vec2 size) -> glm::mat3x2 {
        return { { size.x - SIDE_WIDTH,
                   size.y - FONT_SIZE * 1.5 - SIDE_WIDTH },
                 { SIDE_WIDTH, SIDE_WIDTH },
                 size };
    };
    _fractals[fractals::Fractal::Type::HELP] =
        std::make_unique<fractals::Help>(_info);
    _fractals[fractals::Fractal::Type::MANDELBROT] =
        std::make_unique<fractals::Mandelbrot>(s_fun, _gradient_1d);
    _fractals[fractals::Fractal::Type::JULIA] =
        std::make_unique<fractals::Julia>(s_fun, sp_fun, _gradient_1d);
    _fractals[fractals::Fractal::Type::BURNING_SHIP] =
        std::make_unique<fractals::BurningShip>(s_fun, _gradient_1d);
    _fractals[fractals::Fractal::Type::POWERBROT] =
        std::make_unique<fractals::Powerbrot>(s_fun, sp_fun, _gradient_1d);
    _fractals[fractals::Fractal::Type::NEWTON] =
        std::make_unique<fractals::Newton>(s_fun, _gradient_1d);
    _fractals[fractals::Fractal::Type::BURNING_JULIA] =
        std::make_unique<fractals::BurningJulia>(s_fun, sp_fun, _gradient_1d);
    _fractals[fractals::Fractal::Type::DOUBLE_PENDULUM] =
        std::make_unique<fractals::DoublePendulum>(s_fun);
    _fractals[fractals::Fractal::Type::THREE_BODY] =
        std::make_unique<fractals::ThreeBody>(s_fun);

    _active = _fractals[fractals::Fractal::Type::MANDELBROT].get();
    _focus = _active;
}

void Fractalio::size_callback(int width, int height) {
    glViewport(0, 0, width, height);
    _wsize = { width, height };

    _active->resize(_wsize);
    auto picker = _active->picker();
    if (picker) {
        picker->resize(_wsize);
    }

    _info.resize(_wsize);
    _info.set_transform(
        glm::translate(
            glm::identity<glm::mat4>(), { _wsize.x - SIDE_WIDTH + 10, 20, 0 }
        )
    );

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
        _new_info = true;
    }
}

void Fractalio::scroll_callback(double dx, double dy) {
    auto bot_size = _wsize.y - FONT_SIZE * 1.5;

    if (_last_mouse_pos.x < _wsize.x - SIDE_WIDTH) {
        _drag = _active;
    } else if (_last_mouse_pos.y > bot_size - SIDE_WIDTH &&
               _last_mouse_pos.y < bot_size) {
        if (_active) {
            _drag = _active->picker();
        }
    }

    if (_drag) {
        _drag->scroll(_last_mouse_pos, { dx, dy });
        _new_info = true;
    }
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

void Fractalio::activate(fractals::Fractal::Type typ) {
    if (!_fractals.contains(typ)) {
        std::println(std::cerr, "Unknown fractal type.");
        return;
    }
    _active = _fractals[typ].get();
    _focus = _active;
    _drag = _active;
    _active->resize(_wsize);
    _new_info = true;
    auto picker = _active->picker();
    if (picker) {
        picker->resize(_wsize);
    }
}

} // namespace fio::app
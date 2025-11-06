#include "fractalio.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <format>
#include <iostream>
#include <limits>
#include <memory>
#include <print>
#include <ranges>
#include <utility>
#include <GLFW/glfw3.h>

#include "font.hpp"
#include "gl/gl.hpp"
#include "help.hpp"
#include "mandelbrot/mandelbrot.hpp"
#include "maps.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace fio {

using std::string_view_literals::operator""sv;

constexpr double FPS_INTERVAL = 0.1;

static constexpr std::size_t SIDE_WIDTH = 300;
static constexpr std::size_t FONT_SIZE = 16;

constexpr glm::vec4 DEFAULT_CLEAR_COLOR{ 0.1, 0.1, 0.1, 1 };

Fractalio::Fractalio(std::unique_ptr<glfw::Window> window, const char *font) :
    _window((window->make_context_current(), std::move(window))),
    _font(font, FONT_SIZE),
    _info(_font, std::size_t(FONT_SIZE * 5 / 4)),
    _fps_text(_info),
    _command_input(_info),
    _input_bg({ 0.1, 0.1, 0.1, 1 }) {

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

    init_fractals();

    if (_active) {
        _active->resize({ 0, 0 }, { size.x - SIDE_WIDTH, size.y }, size);
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

    _input_bg.use();
    _input_bg.resize({ 0, size.y - 25 }, { size.x, 25 }, size);

    _command_input.use();
    _command_input.resize(size);
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
                auto text = _active->describe();
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

        if (_new_input) {
            _command_input.clear_text();
            auto pos = _input_text.contains(':')
                           ? glm::vec2{ 5, _wsize.y - FONT_SIZE + 10 }
                           : glm::vec2{ _wsize.x - SIDE_WIDTH + 10,
                                        _wsize.y - FONT_SIZE + 10 };
            _command_input.use();
            _command_input.set_transform(
                glm::translate(glm::identity<glm::mat4>(), { pos, 0 })
            );
            if (_input_text.empty()) {
                _command_input.add_text("Type `?` to show help.", { 0, 0 });
                _command_input.set_color({ .5, .5, .5 });
            } else {
                _command_input.add_text(_input_text, { 0, 0 });
                _command_input.set_color({ 1, 1, 1 });
            }
            _command_input.prepare();
        }
        _new_input = false;

        glClear(GL_COLOR_BUFFER_BIT);
        if (_active) {
            _active->use();
            _active->draw();
        }

        _info.use();
        _info.draw();

        _fps_text.use();
        _fps_text.draw();

        if (_input_text.contains(':')) {
            _input_bg.use();
            _input_bg.draw();
        }
        _command_input.use();
        _command_input.draw();

        _window->swap_buffers();
        glfwPollEvents();
    }
}

void Fractalio::init_fractals() {
    _fractals[Fractal::Type::HELP] = std::make_unique<Help>(_info);
    _fractals[Fractal::Type::MANDELBROT] = std::make_unique<Mandelbrot>();

    _active = _fractals[Fractal::Type::MANDELBROT].get();
}

void Fractalio::size_callback(int width, int height) {
    glViewport(0, 0, width, height);
    _wsize = { width, height };
    if (_active) {
        _active->use();
        _active->resize({ 0, 0 }, { width - SIDE_WIDTH, height }, _wsize);
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

    _input_bg.use();
    _input_bg.resize({ 0, _wsize.y - 25 }, { _wsize.x, 25 }, _wsize);

    _command_input.use();
    auto pos = _input_text.contains(':')
                   ? glm::vec2{ 5, _wsize.y - FONT_SIZE + 10 }
                   : glm::vec2{ _wsize.x - SIDE_WIDTH + 10,
                                _wsize.y - FONT_SIZE + 10 };
    _command_input.resize(_wsize);
    _command_input.set_transform(
        glm::translate(glm::identity<glm::mat4>(), { pos, 0 })
    );
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
        _new_info = true;
    } else if (glfwGetMouseButton(_window->get(), GLFW_MOUSE_BUTTON_RIGHT) ==
               GLFW_PRESS) {
        _active->scale(mouse_pos.y - _last_mouse_pos.y);
        _new_info = true;
    }

    _last_mouse_pos = mouse_pos;
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
        _input_text.clear();
        _new_input = true;
        break;
    case GLFW_KEY_ENTER:
        consume_input();
        _new_input = true;
        break;
    case GLFW_KEY_BACKSPACE:
        if (!_input_text.empty()) {
            _input_text.pop_back();
        }
        _new_input = true;
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

constexpr std::array FOLLOWUP_CMDS{ "r"sv, "g"sv };

static inline bool is_modifier(char c) {
    return std::ranges::contains(std::array{ '=', '+', '-', '*', '/' }, c);
}

static inline bool is_prefix(char c) {
    return std::isdigit(c) || is_modifier(c) || c == '.';
}

static inline bool is_numeric(char c) {
    return std::isdigit(c) || c == '.' || c == '-';
}

static inline std::tuple<char, std::string_view, std::string_view>
split_short_cmd(std::string_view cmd) {
    auto ns = cmd.begin();
    char modifier = 0;
    if (is_modifier(*ns)) {
        modifier = *ns;
        ++ns;
    }
    auto ne =
        std::find_if(ns, cmd.end(), [](char c) { return !is_numeric(c); });
    return { modifier, { ns, ne }, { ne, cmd.end() } };
}

void Fractalio::char_callback(unsigned code) {
    if (code > std::numeric_limits<char>::max()) {
        return;
    }
    auto c = char(code);

    if (_input_text.contains(':') || c == ':') {
        _input_text.push_back(c);
        _new_input = true;
        return;
    }

    if (is_prefix(c)) {
        _input_text.push_back(c);
        _new_input = true;
        return;
    }

    _input_text.push_back(c);
    _new_input = true;

    auto cmd = split_short_cmd(_input_text);
    if (std::ranges::contains(FOLLOWUP_CMDS, std::get<2>(cmd))) {
        return;
    }

    consume_input();
}

void Fractalio::process_input() { }

void Fractalio::consume_input() {
    execute_command(_input_text);
    _input_text.clear();
}

void Fractalio::long_command(std::string_view cmd) {
    auto args = std::views::split(cmd, ' ');
    auto argsi = args.begin();
    cmd = std::string_view(*argsi);
    ++argsi;
    if (cmd == ":x" || cmd == ":exit" || cmd == ":q" || cmd == ":quit") {
        _window->set_should_close(true);
    } else if (cmd.starts_with("::")) {
        const std::string cmd2{ cmd };
        _input_text.clear();
        for (auto c : std::string_view(cmd2).substr(2)) {
            char_callback(c);
        }
        consume_input();
        _last_command = cmd2;
        return;
    } else if (cmd == ":h" || cmd == ":help") {
        activate(Fractal::Type::HELP);
    } else if (cmd == ":vsync") {
        glfwSwapInterval(std::string_view(*argsi) != "off");
    } else {
        std::println(std::cerr, "Unknown command: {}", cmd);
        return;
    }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void Fractalio::execute_command(std::string_view whole_cmd) {
    if (whole_cmd.empty()) {
        return;
    }
    auto [mod, num_str, cmd] = split_short_cmd(whole_cmd);

    std::optional<double> num;
    if (!num_str.empty()) {
        num = 0;
        auto res = std::from_chars(num_str.begin(), num_str.end(), *num);
        if (res.ec != std::errc{}) {
            std::println(
                std::cerr, "Invalid command number prefix `{}`.", num_str
            );
            return;
        }
    }

    if (cmd == "gm") {
        activate(Fractal::Type::MANDELBROT);
    } else if (cmd == "gh" || cmd == "?") {
        activate(Fractal::Type::HELP);
    } else if (!_active) {
        std::println(std::cerr, "Unused command `{}`", cmd);
        _last_command = whole_cmd;
        return;
    } else if (cmd == "i") {
        _active->use();
        _active->map_iterations(
            maps::modified<float>(mod, num, maps::dble<float>)
        );
        _new_info = true;
    } else if (cmd == "I") {
        _active->use();
        _active->map_iterations(
            maps::modified<float>(mod, num, maps::half<float>)
        );
        _new_info = true;
    } else if (cmd == "c") {
        _active->use();
        _active->map_color_count(
            maps::modified<float>(mod, num, maps::dble<float>)
        );
        _new_info = true;
    } else if (cmd == "C") {
        _active->use();
        _active->map_color_count(
            maps::modified<float>(mod, num, maps::half<float>)
        );
        _new_info = true;
    } else if (cmd == "z") {
        _active->use();
        _active->map_scale(
            maps::inverse(maps::modified<double>(mod, num, maps::dble<double>))
        );
        _new_info = true;
    } else if (cmd == "Z") {
        _active->use();
        _active->map_scale(
            maps::inverse(maps::modified<double>(mod, num, maps::half<double>))
        );
        _new_info = true;
    } else if (cmd == "x" || cmd == "L") {
        _active->use();
        _active->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p + _active->scale();
        }));
        _new_info = true;
    } else if (cmd == "X" || cmd == "H") {
        _active->use();
        _active->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p - _active->scale();
        }));
        _new_info = true;
    } else if (cmd == "y" || cmd == "K") {
        _active->use();
        _active->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p + _active->scale();
        }));
        _new_info = true;
    } else if (cmd == "Y" || cmd == "J") {
        _active->use();
        _active->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p - _active->scale();
        }));
        _new_info = true;
    } else if (cmd == "l") {
        _active->use();
        _active->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p + _active->scale() * 0.1;
        }));
        _new_info = true;
    } else if (cmd == "h") {
        _active->use();
        _active->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p - _active->scale() * 0.1;
        }));
        _new_info = true;
    } else if (cmd == "k") {
        _active->use();
        _active->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p + _active->scale() * 0.1;
        }));
        _new_info = true;
    } else if (cmd == "j") {
        _active->use();
        _active->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p - _active->scale() * 0.1;
        }));
        _new_info = true;
    } else if (cmd == "ri") {
        _active->use();
        _active->map_iterations(maps::reset<float>);
        _new_info = true;
    } else if (cmd == "rc") {
        _active->use();
        _active->map_color_count(maps::reset<float>);
        _new_info = true;
    } else if (cmd == "rz") {
        _active->use();
        _active->map_scale(maps::reset<double>);
        _new_info = true;
    } else if (cmd == "rx") {
        _active->use();
        _active->map_x(maps::reset<double>);
        _new_info = true;
    } else if (cmd == "ry") {
        _active->use();
        _active->map_y(maps::reset<double>);
        _new_info = true;
    } else if (cmd == "rp") {
        _active->use();
        _active->map_scale(maps::reset<double>);
        _active->map_x(maps::reset<double>);
        _active->map_y(maps::reset<double>);
        _new_info = true;
    } else if (cmd == "rr") {
        _active->use();
        _active->map_iterations(maps::reset<float>);
        _active->map_color_count(maps::reset<float>);
        _new_info = true;
    } else if (cmd == "R") {
        _active->use();
        _active->map_iterations(maps::reset<float>);
        _active->map_color_count(maps::reset<float>);
        _active->map_scale(maps::reset<double>);
        _active->map_x(maps::reset<double>);
        _active->map_y(maps::reset<double>);
        _new_info = true;
    } else if (cmd == ";") {
        if (_no_recurse) {
            std::println(std::cerr, "Recursive command disallowed.");
            return;
        }
        _no_recurse = true;
        for (auto i = std::size_t(num.value_or(1)); i > 0; --i) {
            execute_command(_last_command);
        }
        _no_recurse = false;
        return;
    } else if (cmd.starts_with(':')) {
        const std::string last_command{ whole_cmd };
        const std::string command{ cmd };
        for (auto i = std::size_t(num.value_or(1)); i > 0; --i) {
            long_command(command);
        }
        _last_command = last_command;
        return;
    } else {
        std::println(std::cerr, "Unknown command `{}`", cmd);
        return;
    }

    _last_command = whole_cmd;
}

void Fractalio::activate(Fractal::Type typ) {
    if (!_fractals.contains(typ)) {
        std::println(std::cerr, "Unknown fractal type.");
        return;
    }
    _active = _fractals[typ].get();
    _active->use();
    _active->resize({ 0, 0 }, { _wsize.x - SIDE_WIDTH, _wsize.y }, _wsize);
    _new_info = true;
}

} // namespace fio
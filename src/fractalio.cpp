#include "fractalio.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <format>
#include <iostream>
#include <limits>
#include <memory>
#include <print>
#include <utility>

#include <GLFW/glfw3.h>

#include "font.hpp"
#include "gl/gl.hpp"
#include "mandelbrot/mandelbrot.hpp"

namespace fio {

using std::string_view_literals::operator""sv;

static constexpr std::size_t SIDE_WIDTH = 300;
static constexpr std::size_t FONT_SIZE = 16;

constexpr glm::vec4 DEFAULT_CLEAR_COLOR{ 0.1, 0.1, 0.1, 1 };

Fractalio::Fractalio(std::unique_ptr<glfw::Window> window, const char *font) :
    _window(std::move(window)),
    _font(font, FONT_SIZE),
    _info(_font, std::size_t(FONT_SIZE * 3 / 2)),
    _fps_text(_info),
    _command_input(_info),
    _input_bg({ 0.1, 0.1, 0.1, 1 }) {

    std::println(std::cerr, "_info program = {}", _info.get_program());
    std::println(std::cerr, "_fps_text program = {}", _fps_text.get_program());
    std::println(
        std::cerr, "_command_input program = {}", _command_input.get_program()
    );

    _window->make_context_current();
    auto size = _window->get_size();
    _wsize = size;
    // glfwSwapInterval(0); // to disable vsync

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

    _active = std::make_unique<Mandelbrot>();
    _active->resize({ 0, 0 }, { size.x - SIDE_WIDTH, size.y }, size);

    _info.use();
    _info.resize(size);

    _fps_text.use();
    _fps_text.resize(size);

    _input_bg.use();
    _input_bg.resize({ 0, size.y - 25 }, { size.x, 25 }, size);

    _command_input.use();
    _command_input.resize(size);
}

void Fractalio::mainloop() {
    _window->make_context_current();

    constexpr double FPS_INTERVAL = 0.1;

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
                _info.add_text(
                    text, { _wsize.x - SIDE_WIDTH + 10, FONT_SIZE + 10 }
                );
            }
            _info.use();
            _info.prepare();
            _new_info = false;
        }

        if (last_time > next_fps) {
            _fps_text.clear_text();
            auto fps = std::format("FPS: {}", std::size_t(1 / delta_time));
            _fps_text.add_text(
                fps, { _wsize.x - SIDE_WIDTH + 10, FONT_SIZE + 10 }
            );
            _fps_text.use();
            _fps_text.prepare();
            next_fps += FPS_INTERVAL;
        }

        if (!_input_text.empty() && _new_input) {
            _command_input.clear_text();
            auto pos = _input_text.contains(':')
                           ? glm::vec2{ 5, _wsize.y - FONT_SIZE + 10 }
                           : glm::vec2{ _wsize.x - SIDE_WIDTH + 10,
                                        _wsize.y - FONT_SIZE + 10 };
            _command_input.add_text(_input_text, pos);
            _command_input.use();
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

        if (!_input_text.empty()) {
            if (_input_text.contains(':')) {
                _input_bg.use();
                _input_bg.draw();
            }
            _command_input.use();
            _command_input.draw();
        }

        _window->swap_buffers();
        glfwPollEvents();
    }
}

void Fractalio::size_callback(int width, int height) {
    glViewport(0, 0, width, height);
    _new_info = true;
    _new_input = true;
    _wsize = { width, height };
    if (_active) {
        _active->use();
        _active->resize({ 0, 0 }, { width - SIDE_WIDTH, height }, _wsize);
    }

    _info.use();
    _info.resize(_wsize);

    _fps_text.use();
    _fps_text.resize(_wsize);

    _input_bg.use();
    _input_bg.resize({ 0, _wsize.y - 25 }, { _wsize.x, 25 }, _wsize);

    _command_input.use();
    _command_input.resize(_wsize);
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
    default:
        break;
    }
}

constexpr std::array FOLLOWUP_CMDS{ "r"sv };

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
    } else {
        std::println(std::cerr, "Unknown command: {}", cmd);
        return;
    }
}

namespace maps {
static inline std::function<float(float)> value(float v) {
    return [=](float) { return v; };
}

static inline float dble(float v) {
    return std::max(v * 2.F, 1.F);
}

static inline float half(float v) {
    return v / 2;
}

static inline std::function<float(float)> add(float v) {
    return [=](float p) { return p + v; };
}

static inline std::function<float(float)> mul(float v) {
    return [=](float p) { return std::max(p * v, 1.F); };
}

static inline float reset(float) {
    return NAN;
}

static inline std::function<float(float)> modified(
    char modifier, std::optional<float> num, std::function<float(float)> def
) {
    switch (modifier) {
    case '+':
        return add(num.value_or(1.));
    case '-':
        return add(-num.value_or(1.));
    case '*':
        return mul(num.value_or(2.));
    case '/':
        return mul(1 / num.value_or(2.));
    case '=':
        if (num) {
            return value(*num);
        }
        return reset;
    default:
        if (num) {
            return value(*num);
        }
        return def;
    }
}
} // namespace maps

void Fractalio::execute_command(std::string_view whole_cmd) {
    if (whole_cmd.empty()) {
        return;
    }
    auto [mod, num_str, cmd] = split_short_cmd(whole_cmd);

    std::optional<float> num;
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

    if (!_active) {
        std::println(std::cerr, "Unused command.");
        return;
    }

    if (cmd == "i") {
        _active->use();
        _active->map_iterations(maps::modified(mod, num, maps::dble));
        _new_info = true;
    } else if (cmd == "I") {
        _active->use();
        _active->map_iterations(maps::modified(mod, num, maps::half));
        _new_info = true;
    } else if (cmd == "c") {
        _active->use();
        _active->map_color_count(maps::modified(mod, num, maps::dble));
        _new_info = true;
    } else if (cmd == "C") {
        _active->use();
        _active->map_color_count(maps::modified(mod, num, maps::half));
        _new_info = true;
    } else if (cmd == "ri") {
        _active->use();
        _active->map_iterations(maps::reset);
        _new_info = true;
    } else if (cmd == "rc") {
        _active->use();
        _active->map_color_count(maps::reset);
        _new_info = true;
    } else if (cmd == "rr") {
        _active->use();
        _active->map_iterations(maps::reset);
        _active->map_color_count(maps::reset);
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

} // namespace fio
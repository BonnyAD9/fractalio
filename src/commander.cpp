#include "commander.hpp"

#include <algorithm>
#include <ranges>

#include <GLFW/glfw3.h>

#include "fractalio.hpp"
#include "maps.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace fio {

using std::string_view_literals::operator""sv;

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

Commander::Commander(Fractalio &app, const TextRenderer &text_cfg) :
    _app(app), _input(text_cfg), _bg(Fractalio::DEFAULT_CLEAR_COLOR) { }

void Commander::cancel() {
    if (_text.empty()) {
        _app._focus = _app._active;
        _app._new_info = true;
    } else {
        _text.clear();
    }
    _new_input = true;
}

void Commander::prepare() {
    if (_new_input) {
        _input.clear_text();
        auto pos = _text.contains(':')
                       ? glm::vec2{ 5, _wsize.y - Fractalio::FONT_SIZE + 10 }
                       : glm::vec2{ _wsize.x - Fractalio::SIDE_WIDTH + 10,
                                    _wsize.y - Fractalio::FONT_SIZE + 10 };
        _input.use();
        _input.set_transform(
            glm::translate(glm::identity<glm::mat4>(), { pos, 0 })
        );
        if (_text.empty()) {
            _input.add_text("Type `?` to show help.", { 0, 0 });
            _input.set_color({ .5, .5, .5 });
        } else {
            _input.add_text(_text, { 0, 0 });
            _input.set_color({ 1, 1, 1 });
        }
        _input.prepare();
    }
    _new_input = false;
}

void Commander::draw() {
    if (_text.contains(':')) {
        _bg.use();
        _bg.draw();
    }
    _input.use();
    _input.draw();
}

void Commander::resize(glm::vec2 size) {
    _wsize = size;
    _bg.use();
    _bg.resize({ 0, _wsize.y - 25 }, { _wsize.x, 25 }, _wsize);

    _input.use();
    auto pos = _text.contains(':')
                   ? glm::vec2{ 5, _wsize.y - Fractalio::FONT_SIZE + 10 }
                   : glm::vec2{ _wsize.x - Fractalio::SIDE_WIDTH + 10,
                                _wsize.y - Fractalio::FONT_SIZE + 10 };
    _input.resize(_wsize);
    _input.set_transform(
        glm::translate(glm::identity<glm::mat4>(), { pos, 0 })
    );
}

void Commander::char_in(char c) {
    if (_text.contains(':') || c == ':') {
        _text.push_back(c);
        _new_input = true;
        return;
    }

    if (is_prefix(c)) {
        _text.push_back(c);
        _new_input = true;
        return;
    }

    _text.push_back(c);
    _new_input = true;

    auto cmd = split_short_cmd(_text);
    if (std::ranges::contains(FOLLOWUP_CMDS, std::get<2>(cmd))) {
        return;
    }

    consume();
}

void Commander::consume() {
    execute_command(_text);
    _text.clear();
    _new_input = true;
}

void Commander::long_command(std::string_view cmd) {
    auto argss = std::views::split(cmd, ' ');
    auto args = argss.begin();

    cmd = std::string_view(*args);
    ++args;
    if (cmd == ":x" || cmd == ":exit" || cmd == ":q" || cmd == ":quit") {
        _app._window->set_should_close(true);
    } else if (cmd.starts_with("::")) {
        const std::string cmd2{ cmd };
        _text.clear();
        for (auto c : std::string_view(cmd2).substr(2)) {
            char_in(c);
        }
        consume();
        _last = cmd2;
        return;
    } else if (cmd == ":h" || cmd == ":help") {
        _app.activate(Fractal::Type::HELP);
    } else if (cmd == ":vsync") {
        if (args == argss.end()) {
            glfwSwapInterval(0);
        }
        glfwSwapInterval(std::string_view(*args) != "off");
    } else {
        std::println(std::cerr, "Unknown command: {}", cmd);
        return;
    }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void Commander::execute_command(std::string_view whole_cmd) {
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
        _app.activate(Fractal::Type::MANDELBROT);
    } else if (cmd == "gh" || cmd == "?") {
        _app.activate(Fractal::Type::HELP);
    } else if (cmd == "gj") {
        _app.activate(Fractal::Type::JULIA);
    } else if (cmd == "G") {
        _app.activate(Fractal::Type(std::size_t(num.value_or(0))));
    } else if (cmd == " ") {
        _app._focus = _app._active->picker();
        if (!_app._focus) {
            _app._focus = _app._active;
        }
        _app._new_info = true;
    } else if (cmd == "i") {
        _app._focus->use();
        _app._focus->map_iterations(
            maps::modified<float>(mod, num, maps::dble<float>)
        );
        _app._new_info = true;
    } else if (cmd == "I") {
        _app._focus->use();
        _app._focus->map_iterations(
            maps::modified<float>(mod, num, maps::half<float>)
        );
        _app._new_info = true;
    } else if (cmd == "c") {
        _app._focus->use();
        _app._focus->map_color_count(
            maps::modified<float>(mod, num, maps::dble<float>)
        );
        _app._new_info = true;
    } else if (cmd == "C") {
        _app._focus->use();
        _app._focus->map_color_count(
            maps::modified<float>(mod, num, maps::half<float>)
        );
        _app._new_info = true;
    } else if (cmd == "z") {
        _app._focus->use();
        _app._focus->map_scale(
            maps::inverse(maps::modified<double>(mod, num, maps::dble<double>))
        );
        _app._new_info = true;
    } else if (cmd == "Z") {
        _app._focus->use();
        _app._focus->map_scale(
            maps::inverse(maps::modified<double>(mod, num, maps::half<double>))
        );
        _app._new_info = true;
    } else if (cmd == "x" || cmd == "L") {
        _app._focus->use();
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "X" || cmd == "H") {
        _app._focus->use();
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "y" || cmd == "K") {
        _app._focus->use();
        _app._focus->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "Y" || cmd == "J") {
        _app._active->use();
        _app._active->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "l") {
        _app._focus->use();
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "h") {
        _app._focus->use();
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "k") {
        _app._focus->use();
        _app._focus->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "j") {
        _app._focus->use();
        _app._focus->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "ri") {
        _app._focus->use();
        _app._focus->map_iterations(maps::reset<float>);
        _app._new_info = true;
    } else if (cmd == "rc") {
        _app._focus->use();
        _app._focus->map_color_count(maps::reset<float>);
        _app._new_info = true;
    } else if (cmd == "rz") {
        _app._focus->use();
        _app._focus->map_scale(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "rx") {
        _app._focus->use();
        _app._focus->map_x(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "ry") {
        _app._focus->use();
        _app._focus->map_y(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "rp") {
        _app._focus->use();
        _app._focus->map_scale(maps::reset<double>);
        _app._focus->map_x(maps::reset<double>);
        _app._focus->map_y(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "rr") {
        _app._focus->use();
        _app._focus->map_iterations(maps::reset<float>);
        _app._focus->map_color_count(maps::reset<float>);
        _app._new_info = true;
    } else if (cmd == "R") {
        _app._focus->use();
        _app._focus->map_iterations(maps::reset<float>);
        _app._focus->map_color_count(maps::reset<float>);
        _app._focus->map_scale(maps::reset<double>);
        _app._focus->map_x(maps::reset<double>);
        _app._focus->map_y(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == ";") {
        if (_no_recurse) {
            std::println(std::cerr, "Recursive command disallowed.");
            return;
        }
        _no_recurse = true;
        for (auto i = std::size_t(num.value_or(1)); i > 0; --i) {
            execute_command(_last);
        }
        _no_recurse = false;
        return;
    } else if (cmd.starts_with(':')) {
        const std::string last_command{ whole_cmd };
        const std::string command{ cmd };
        for (auto i = std::size_t(num.value_or(1)); i > 0; --i) {
            long_command(command);
        }
        _last = last_command;
        return;
    } else {
        std::println(std::cerr, "Unknown command `{}`", cmd);
        return;
    }

    _last = whole_cmd;
}

} // namespace fio
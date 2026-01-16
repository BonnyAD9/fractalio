#include "commander.hpp"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <print>
#include <ranges>
#include <sstream>

#include <GLFW/glfw3.h>

#include "../gradient.hpp"
#include "../pareg/pareg.hpp"
#include "../pareg/split_on_whitespace.hpp"
#include "fractalio.hpp"
#include "maps.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>

namespace fio::app {

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

Commander::Commander(Fractalio &app, const gui::Text &text_cfg) :
    _app(app),
    _input(text_cfg),
    _bg(Fractalio::DEFAULT_CLEAR_COLOR, [=](glm::vec2 size) -> glm::mat3x2 {
        return { { 0, size.y - 25 }, { size.x, 25 }, size };
    }) { }

void Commander::cancel() {
    if (_text.empty()) {
        _app._focus = _app._active;
        _app._new_info = true;
    } else {
        _text.clear();
    }
    _draw_flags |= NEW_INPUT;
}

void Commander::draw() {
    const bool long_cmd = _text.contains(':');

    if (_draw_flags & NEW_INPUT) {
        _input.clear_text();
        auto pos = long_cmd
                       ? glm::vec2{ 5, _wsize.y - Fractalio::FONT_SIZE + 10 }
                       : glm::vec2{ _wsize.x - Fractalio::SIDE_WIDTH + 10,
                                    _wsize.y - Fractalio::FONT_SIZE + 10 };
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
    }

    _draw_flags = 0;
    if (_text.contains(':')) {
        _bg.draw();
    }
    _input.draw();
}

void Commander::resize(glm::vec2 size) {
    _wsize = size;
    _bg.resize(size);

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
    _draw_flags |= NEW_INPUT;

    if (c == '\n') {
        consume();
        return;
    }

    if (_text.contains(':') || c == ':') {
        _text.push_back(c);
        return;
    }

    if (is_prefix(c)) {
        _text.push_back(c);
        return;
    }

    _text.push_back(c);

    auto cmd = split_short_cmd(_text);
    if (std::ranges::contains(FOLLOWUP_CMDS, std::get<2>(cmd))) {
        return;
    }

    consume();
}

void Commander::consume() {
    _draw_flags |= NEW_INPUT;
    std::string text2(_text);

    try {
        execute_command(_text);
    } catch (std::exception &ex) {
        std::println(
            std::cerr, "Failed to execute command `{}`: {}", text2, ex.what()
        );
    }
    _text.clear();
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
void Commander::long_command(std::string_view cmd) {
    auto arg = pareg::SplitWhitespace(cmd);
    auto args = pareg::Pareg(arg.begin(), arg.end());

    cmd = std::string_view(*args);
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
        _app.activate(fractals::Fractal::Type::HELP);
    } else if (cmd == ":vsync") {
        if (args.empty()) {
            glfwSwapInterval(1);
        } else {
            glfwSwapInterval(args.next_arg<int>());
        }
    } else if (cmd == ":fps") {
        if (args.empty()) {
            _app._min_interval = 0;
        } else {
            auto fps = args.next_arg<double>();
            _app._min_interval = fps == 0 ? 0 : 1 / fps;
        }
    } else if (cmd == ":flags") {
        auto mask = args.next_arg<GLuint>();
        auto value = args.next_arg<GLuint>();
        _app._focus->set_flags(mask, value);
        _app._new_info = true;
    } else if (cmd == ":flag") {
        _app._focus->flag(args.next_arg<std::string_view>());
        _app._new_info = true;
    } else if (cmd == ":par" || cmd == ":parameter") {
        auto x = args.next_arg<double>();
        auto y = args.next_arg<double>();
        std::size_t par = 0;
        if (!args.empty()) {
            par = std::size_t(x);
            x = y;
            y = args.next_arg<double>();
        }
        _app._focus->map_parameter_x(par, maps::value(x));
        _app._focus->map_parameter_y(par, maps::value(y));
        _app._new_info = true;
    } else if (cmd == ":grad1" || cmd == ":gradient1") {
        auto v = args.next_arg<std::string_view>();
        std::vector<glm::u8vec3> grad(256);
        if (v == "ultra-fractal") {
            gradient::ultra_fractal(grad);
        } else if (v == "grayscale") {
            gradient::grayscale(grad);
        } else if (v == "burn") {
            gradient::burn(grad);
        } else if (v == "monokai") {
            gradient::monokai(grad);
        } else if (v == "rgb") {
            gradient::rgb(grad);
        } else if (v == "cmy") {
            gradient::cmy(grad);
        } else {
            auto siz = args.cur<std::size_t>();
            grad.resize(siz);
            auto pts = args.next_arg<
                std::vector<std::pair<float, glm::u8vec3>>,
                float>();
            gradient::linear_gradient(pts, grad);
        }
        _app._gradient_1d.bind(GL_TEXTURE_1D);
        gl::tex_image_1d(grad);
    } else if (cmd == ":set") {
        auto par = args.next_arg<std::string_view>();
        if (args.empty()) {
            _app._focus->set(par, std::optional<double>(std::nullopt));
            _app._focus->set(par, std::optional<glm::dvec2>(std::nullopt));
        } else {
            auto v1 = args.next_arg<double>();
            if (args.empty()) {
                _app._focus->set(par, v1);
            } else {
                auto v2 = args.next_arg<double>();
                _app._focus->set(par, { { v1, v2 } });
            }
        }
        _app._new_info = true;
    } else if (cmd == ":save") {
        std::string state(":fractal\n");
        _app._active->save_state(state);
        auto picker = _app._active->picker();
        if (picker) {
            state += ":picker\n";
            picker->save_state(state);
            state += ":fractal\n";
        }
        if (args.empty()) {
            _app._window->set_clipboard_string(state.c_str());
        } else {
            auto f = args.next_arg<std::string>();
            std::ofstream file;
            file.open(f);
            file << state;
            file.close();
        }
    } else if (cmd == ":load") {
        std::string state;
        if (args.empty()) {
            state += _app._window->get_clipboard_string();
        } else {
            auto f = args.next_arg<std::string>();
            std::ifstream file;
            file.open(f);
            std::ostringstream os;
            os << file.rdbuf();
            state = os.str();
        }
        _text.clear();
        for (auto c : state) {
            char_in(c);
        }
        consume();
        _last = cmd;
        return;
    } else if (cmd == ":picker") {
        _app._focus = _app._active->picker();
        if (!_app._focus) {
            _app._focus = _app._active;
        }
        _app._new_info = true;
    } else if (cmd == ":fractal") {
        _app._focus = _app._active;
        _app._new_info = true;
    } else if (cmd == ":precision") {
        auto p = args.next_arg<std::size_t>();
        if (p == 1) {
            _app._focus->map_use_double(maps::value(false));
            _app._new_info = true;
        } else if (p == 2) {
            _app._focus->map_use_double(maps::value(true));
            _app._new_info = true;
        }
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
        _app.activate(fractals::Fractal::Type::MANDELBROT);
    } else if (cmd == "gh" || cmd == "?") {
        _app.activate(fractals::Fractal::Type::HELP);
    } else if (cmd == "gj") {
        _app.activate(fractals::Fractal::Type::JULIA);
    } else if (cmd == "gs") {
        _app.activate(fractals::Fractal::Type::BURNING_SHIP);
    } else if (cmd == "gn") {
        _app.activate(fractals::Fractal::Type::NEWTON);
    } else if (cmd == "gp") {
        _app.activate(fractals::Fractal::Type::DOUBLE_PENDULUM);
    } else if (cmd == "G") {
        _app.activate(fractals::Fractal::Type(std::size_t(num.value_or(0))));
    } else if (cmd == " ") {
        _app._focus = _app._active->picker();
        if (!_app._focus) {
            _app._focus = _app._active;
        }
        _app._new_info = true;
    } else if (cmd == "i") {
        _app._focus->map_iterations(
            maps::modified<float>(mod, num, maps::dble<float>)
        );
        _app._new_info = true;
    } else if (cmd == "I") {
        _app._focus->map_iterations(
            maps::modified<float>(mod, num, maps::half<float>)
        );
        _app._new_info = true;
    } else if (cmd == "c") {
        _app._focus->map_color_count(
            maps::modified<float>(mod, num, maps::dble<float>)
        );
        _app._new_info = true;
    } else if (cmd == "C") {
        _app._focus->map_color_count(
            maps::modified<float>(mod, num, maps::half<float>)
        );
        _app._new_info = true;
    } else if (cmd == "z") {
        _app._focus->map_scale(
            maps::inverse(maps::modified<double>(mod, num, maps::dble<double>))
        );
        _app._new_info = true;
    } else if (cmd == "Z") {
        _app._focus->map_scale(
            maps::inverse(maps::modified<double>(mod, num, maps::half<double>))
        );
        _app._new_info = true;
    } else if (cmd == "x" || cmd == "L") {
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "X" || cmd == "H") {
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "y" || cmd == "K") {
        _app._focus->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "Y" || cmd == "J") {
        _app._active->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale();
        }));
        _app._new_info = true;
    } else if (cmd == "l") {
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "h") {
        _app._focus->map_x(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "k") {
        _app._focus->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p + _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "j") {
        _app._focus->map_y(maps::modified<double>(mod, num, [&](double p) {
            return p - _app._active->scale() * 0.1;
        }));
        _app._new_info = true;
    } else if (cmd == "t") {
        _app._focus->map_time(maps::modified<double>(mod, num, [&](double p) {
            return p;
        }));
        _app._new_info = true;
    } else if (cmd == "s") {
        _app._focus->map_step(
            maps::modified<double>(mod, num, maps::half<double>)
        );
        _app._new_info = true;
    } else if (cmd == "S") {
        _app._focus->map_step(
            maps::modified<double>(mod, num, maps::dble<double>)
        );
        _app._new_info = true;
    } else if (cmd == "v") {
        _app._focus->map_speed(
            maps::modified<double>(mod, num, maps::half<double>)
        );
        _app._new_info = true;
    } else if (cmd == "V") {
        _app._focus->map_speed(
            maps::modified<double>(mod, num, maps::dble<double>)
        );
        _app._new_info = true;
    } else if (cmd == "ri") {
        _app._focus->map_iterations(maps::reset<float>);
        _app._new_info = true;
    } else if (cmd == "rc") {
        _app._focus->map_color_count(maps::reset<float>);
        _app._new_info = true;
    } else if (cmd == "rz") {
        _app._focus->map_scale(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "rx") {
        _app._focus->map_x(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "ry") {
        _app._focus->map_y(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "rp") {
        _app._focus->map_scale(maps::reset<double>);
        _app._focus->map_x(maps::reset<double>);
        _app._focus->map_y(maps::reset<double>);
        _app._new_info = true;
    } else if (cmd == "rr") {
        _app._focus->map_iterations(maps::reset<float>);
        _app._focus->map_color_count(maps::reset<float>);
        _app._new_info = true;
    } else if (cmd == "R") {
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

} // namespace fio::app

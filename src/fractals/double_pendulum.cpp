#include "double_pendulum.hpp"

#include "chaotic_fractal.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "double_pendulum.frag"
    , 0
};

DoublePendulum::DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    ChaoticFractal(FRAGMENT_SHADER, std::move(s_fun)) {
    auto &prog = program();

    _loc_g = prog.uniform_location("g");
    _loc_m1 = prog.uniform_location("m1");
    _loc_m2 = prog.uniform_location("m2");
    _loc_l1 = prog.uniform_location("l1");
    _loc_l2 = prog.uniform_location("l2");
}

std::string DoublePendulum::describe() {
    auto res = describe_part("Double pendulum");
    res += std::format("\n  gravity: {}", _g);
    res += std::format("\n  mass 1: {}", _m1);
    res += std::format("\n  mass 2: {}", _m2);
    res += std::format("\n  length 1: {}", _l1);
    res += std::format("\n  length 2: {}", _l2);
    res += std::format("\n  flags: {:x}", flags());

    std::string_view coloring;
    switch (flags() & 0xF) {
    default:
        coloring = "position";
        break;
    case 1:
        coloring = "velocity";
        break;
    case 2:
        coloring = "acceleration";
        break;
    }

    res += std::format("\n    coloring: {}", coloring);
    res += std::format(
        "\n    method: {}", (flags() & 0xF0) == 0 ? "euler" : "runge kutta 4"
    );
    res += std::format("\n    init position: {}", (bool)(flags() & 0x100));

    return res;
}

void DoublePendulum::set(std::string_view param, std::optional<double> value) {
    if (param == "g" || param == "gravity") {
        _g = float(value.value_or(9.8));
    } else if (param == "m1" || param == "mass1") {
        _m1 = float(value.value_or(1));
    } else if (param == "m2" || param == "mass2") {
        _m2 = float(value.value_or(1));
    } else if (param == "l1" || param == "length1") {
        _l1 = float(value.value_or(1));
    } else if (param == "l2" || param == "length2") {
        _l2 = float(value.value_or(1));
    } else {
        ChaoticFractal::set(param, value);
        return;
    }
    add_draw_flag(NEW_SPECIAL);
}

void DoublePendulum::update_parameters(bool force) {
    ChaoticFractal::update_parameters(force);

    auto &prog = program();
    auto dflags = draw_flags();

    if (dflags & NEW_SPECIAL) {
        prog.uniform(_loc_g, _g);
        prog.uniform(_loc_m1, _m1);
        prog.uniform(_loc_m2, _m2);
        prog.uniform(_loc_l1, _l1);
        prog.uniform(_loc_l2, _l2);
    }
}

} // namespace fio::fractals
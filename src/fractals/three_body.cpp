#include "three_body.hpp"

#include "chaotic_fractal.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "three_body.frag"
    , 0
};

ThreeBody::ThreeBody(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    ChaoticFractal(FRAGMENT_SHADER, std::move(s_fun), { -1, 1, 1, -1 }, 3),
    _picker({ { -0.5, 0 }, { 0.5, 0 } }) {
    auto &prog = program();

    _loc_hover = prog.uniform_location("hover");

    _loc_g = prog.uniform_location("g");
    _loc_m0 = prog.uniform_location("m0");
    _loc_m1 = prog.uniform_location("m1");
    _loc_m2 = prog.uniform_location("m2");
    _loc_sscale = prog.uniform_location("sscale");
    _loc_stabilization = prog.uniform_location("stabilization");
    _loc_init1 = prog.uniform_location("init1");
    _loc_init2 = prog.uniform_location("init2");
}

std::string ThreeBody::describe() {
    auto res = describe_part("Three body");
    res += std::format("\n gravity: {}", _g);
    res += std::format("\n mass red: {}", _m0);
    res += std::format("\n mass green: {}", _m1);
    res += std::format("\n mass blue: {}", _m2);
    res += std::format("\n space scale: {}", _sscale);
    res += std::format("\n stabilization: {}", _stabilization);
    res += std::format("\n flags: {:x}", flags());

    std::string_view coloring;
    switch (flags() & 0xF) {
    default:
        coloring = "relative distance";
        break;
    case 1:
        coloring = "distance";
        break;
    case 2:
        coloring = "velocity";
        break;
    case 3:
        coloring = "angle";
        break;
    case 4:
        coloring = "relative angle";
        break;
    case 5:
        coloring = "closest angle";
        break;
    }

    res += std::format("\n    coloring: {}", coloring);
    res += std::format(
        "\n    method: {}", (flags() & 0xF0) == 0 ? "euler" : "runge kutta 4"
    );
    res += std::format("\n    relative: {}", !(flags() & 0x100));
    res += std::format("\n    overlay: {}", !(flags() & 0x200));
    return res;
}

void ThreeBody::set(std::string_view param, std::optional<double> value) {
    if (param == "g" || param == "gravity") {
        _g = float(value.value_or(6.67430e-11));
    } else if (param == "m0" || param == "mass0" || param == "mr" ||
               param == "mass-red") {
        _m0 = float(value.value_or(100));
    } else if (param == "m1" || param == "mass1" || param == "mg" ||
               param == "mass-green") {
        _m1 = float(value.value_or(100));
    } else if (param == "m2" || param == "mass2" || param == "mb" ||
               param == "mass-blue") {
        _m2 = float(value.value_or(100));
    } else if (param == "sscale" || param == "space-scale") {
        _sscale = float(value.value_or(0.001));
    } else if (param == "stabilization") {
        _stabilization = float(value.value_or(0.00000001));
    } else {
        ChaoticFractal::set(param, value);
        return;
    }
    add_draw_flag(NEW_SPECIAL);
}

void ThreeBody::update_parameters(bool force) {
    auto &prog = program();
    auto dflags = draw_flags();

    if (_picker.new_par()) {
        add_draw_flag(NEW_USE_DOUBLE);

        auto inits = _picker.dpars();
        prog.uniform(_loc_init1, glm::vec2(inits[0]));
        prog.uniform(_loc_init2, glm::vec2(inits[1]));
    }

    ChaoticFractal::update_parameters(force);

    prog.uniform(_loc_hover, _hover);

    if (dflags & NEW_SPECIAL) {
        prog.uniform(_loc_g, _g);
        prog.uniform(_loc_m0, _m0);
        prog.uniform(_loc_m1, _m1);
        prog.uniform(_loc_m2, _m2);
        prog.uniform(_loc_sscale, _sscale);
        prog.uniform(_loc_stabilization, _stabilization);
    }

    _picker.reset_flags();
}

} // namespace fio::fractals
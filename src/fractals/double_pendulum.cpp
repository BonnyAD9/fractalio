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
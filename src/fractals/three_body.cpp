#include "three_body.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "three_body.frag"
    , 0
};

ThreeBody::ThreeBody(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    ChaoticFractal(FRAGMENT_SHADER, std::move(s_fun), { -1, 1, 1, -1 }, 3) {
    auto &prog = program();

    _loc_hover = prog.uniform_location("hover");

    _loc_g = prog.uniform_location("g");
    _loc_m0 = prog.uniform_location("m0");
    _loc_m1 = prog.uniform_location("m1");
    _loc_m2 = prog.uniform_location("m2");
    _loc_sscale = prog.uniform_location("sscale");
    _loc_stabilization = prog.uniform_location("stabilization");
}

void ThreeBody::set(std::string_view param, std::optional<float> value) {
    if (param == "g" || param == "gravity") {
        _g = value.value_or(6.67430e-11);
    } else if (param == "m0" || param == "mass0" || param == "mr" ||
               param == "mass-red") {
        _m0 = value.value_or(100);
    } else if (param == "m1" || param == "mass1" || param == "mg" ||
               param == "mass-green") {
        _m1 = value.value_or(100);
    } else if (param == "m2" || param == "mass2" || param == "mb" ||
               param == "mass-blue") {
        _m2 = value.value_or(100);
    } else if (param == "sscale" || param == "space-scale") {
        _sscale = value.value_or(0.001);
    } else if (param == "stabilization") {
        _stabilization = value.value_or(0.00000001);
    } else {
        return;
    }
    add_draw_flag(NEW_SPECIAL);
}

} // namespace fio::fractals
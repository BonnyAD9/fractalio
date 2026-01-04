#include "gravity_basins.hpp"

#include <format>
#include <functional>

#include "../app/maps.hpp"
#include "../glsl/preprocess.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

static constexpr double SQRT3O2 = 0.8660254037844386;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "gravity_basins.frag"
    , 0
};

static std::string prepare_shader(std::size_t root_cnt) {
    std::string res;
    glsl::preprocess_defines(
        res, FRAGMENT_SHADER, { { "max_bodies", std::format("{}", root_cnt) } }
    );
    return res;
}

GravityBasins::GravityBasins(
    std::function<glm::mat3x2(glm::vec2)> s_fun,
    gl::Texture &gradient,
    std::size_t root_cnt
) :
    ComplexFractal(prepare_shader(root_cnt), std::move(s_fun), gradient),
    _picker({ { 1, 0 }, { -.5, -SQRT3O2 }, { -.5, SQRT3O2 } }, root_cnt) {
    auto &prog = program();
    prog.use();
    _loc_bodies = prog.uniform_location("bodies");
    _loc_body_cnt = prog.uniform_location("body_cnt");
    _loc_time = prog.uniform_location("time");
    _loc_step_size = prog.uniform_location("step_size");
    set_flags(0xF, 1);
    map_color_count(app::maps::value(12));
}

std::string GravityBasins::describe() {
    auto desc = describe_part("Gravity basins");
    desc += std::format("  time: {}\n", int(_time));
    desc += std::format("  step size: {:f}\n", _step_size);
    desc += "\n  bodies:\n";
    for (auto &r : _picker.dpars()) {
        desc += std::format("    [{:.6}, {:.6}]\n", r.x, r.y);
    }
    return desc;
}

void GravityBasins::update_parameters(bool force) {
    ComplexFractal::update_parameters(force);

    auto &prog = program();
    auto flags = draw_flags();

    if (_picker.update_parameter(force, *this)) {
        auto bodies = _picker.pars();
        prog.uniform(_loc_bodies, bodies);
        prog.uniform(_loc_body_cnt, GLuint(bodies.size()));
    }

    if (flags & NEW_TIME) {
        prog.uniform(_loc_time, float(_time));
    }

    if (flags & NEW_STEP_SIZE) {
        prog.uniform(_loc_step_size, float(_step_size));
    }
}

} // namespace fio::fractals
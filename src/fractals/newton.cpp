#include "newton.hpp"

#include <format>
#include <functional>

#include "../app/maps.hpp"
#include "../glsl/preprocess.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

static constexpr double SQRT3O2 = 0.8660254037844386;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "newton.frag"
    , 0
};

static std::string prepare_shader(std::size_t root_cnt) {
    std::string res;
    glsl::preprocess_defines(
        res, FRAGMENT_SHADER, { { "max_roots", std::format("{}", root_cnt) } }
    );
    return res;
}

Newton::Newton(
    std::function<glm::mat3x2(glm::vec2)> s_fun,
    gl::Texture &gradient,
    std::size_t root_cnt
) :
    IterativeFractal(prepare_shader(root_cnt), std::move(s_fun), gradient),
    _picker({ { 1, 0 }, { -.5, -SQRT3O2 }, { -.5, SQRT3O2 } }, root_cnt) {
    auto &prog = program();
    prog.use();
    _loc_coefs = prog.uniform_location("coefs");
    _loc_roots = prog.uniform_location("roots");
    _loc_root_cnt = prog.uniform_location("root_cnt");
    set_flags(0xF, 1);
    map_color_count(app::maps::value(12));
}

std::string Newton::describe() {
    auto desc = describe_part("Newton fractal");
    desc += "\n  roots:";
    for (auto &r : _picker.dpars()) {
        desc += std::format("\n    {:.6} + {:.6}i", r.x, r.y);
    }
    desc += std::format("\n  flags: {:x}", flags());

    std::string_view coloring;
    switch (flags() & 0xF) {
    default:
        coloring = "flat";
        break;
    case 1:
        coloring = "bright";
        break;
    case 2:
        coloring = "dark";
        break;
    case 3:
        coloring = "iteration";
        break;
    case 4:
        coloring = "log-iteration";
        break;
    }

    desc += std::format("\n    coloring: {}", coloring);
    desc += std::format("\n    smooth: {}", !(flags() & 0x10));
    desc += std::format("\n    overlay: {}", !(flags() & 0x20));
    return desc;
}

void Newton::update_parameters(bool force) {
    IterativeFractal::update_parameters(force);

    auto &prog = program();

    if (_picker.update_parameter(force, *this)) {
        auto roots = _picker.pars();
        auto coefs = get_coefs(roots);
        prog.uniform(_loc_coefs, coefs);
        prog.uniform(_loc_roots, roots);
        prog.uniform(_loc_root_cnt, GLuint(roots.size()));
    }
}

static glm::vec2 cmul(glm::vec2 a, glm::vec2 b) {
    return { a.x * b.x - a.y * b.y, a.x * b.y + b.x * a.y };
}

std::vector<glm::vec2> Newton::get_coefs(std::span<glm::vec2> roots) {
    std::vector<glm::vec2> coefs{ roots.size() + 1, { 0, 0 } };
    coefs[roots.size()] = { 1, 0 };

    for (std::size_t i = roots.size(); i > 0; --i) {
        for (std::size_t j = i - 1; j < roots.size(); ++j) {
            coefs[j] = cmul(coefs[j], -roots[i - 1]) + coefs[j + 1];
        }
        coefs[roots.size()] = cmul(coefs[roots.size()], -roots[i - 1]);
    }
    return coefs;
}

} // namespace fio::fractals
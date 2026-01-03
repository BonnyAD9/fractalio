#include "three_body.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "three_body.frag"
    , 0
};

ThreeBody::ThreeBody(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    ChaoticFractal(
        FRAGMENT_SHADER,
        std::move(s_fun),
        { -1, 1, 1, -1 },
        6,
        { GL_RGB32F, GL_RGB, GL_FLOAT }
    ) {
    auto &prog = program();
    _loc_hover = prog.uniform_location("hover");
}

} // namespace fio::fractals
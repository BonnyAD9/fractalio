#include "burning_ship.hpp"

namespace fio::fractals::pickers {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "burning_ship.frag"
    , 0
};

BurningShip::BurningShip(
    std::function<glm::mat3x2(glm::vec2)> s_fun, gl::Texture &gradient
) :
    IterativeFractal(FRAGMENT_SHADER, std::move(s_fun), gradient) {
    auto &prog = program();
    _par_loc = prog.uniform_location("par");
}

} // namespace fio::fractals::pickers
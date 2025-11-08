#include "burning_ship.hpp"

namespace fio {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "shader.frag"
    , 0
};

BurningShip::BurningShip() : ComplexFractal(FRAGMENT_SHADER) { }

} // namespace fio
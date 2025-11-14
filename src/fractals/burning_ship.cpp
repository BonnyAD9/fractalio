#include "burning_ship.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "burning_ship.frag"
    , 0
};

BurningShip::BurningShip() : ComplexFractal(FRAGMENT_SHADER) { }

} // namespace fio::fractals
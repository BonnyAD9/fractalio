#include "burning_julia.hpp"

#include <memory>

#include "pickers/burning_ship.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "burning_julia.frag"
    , 0
};

BurningJulia::BurningJulia(
    std::function<glm::mat3x2(glm::vec2)> s_fun,
    std::function<glm::mat3x2(glm::vec2)> sp_fun
) :
    IterativeFractal(FRAGMENT_SHADER, std::move(s_fun)) {
    auto &prog = program();
    prog.use();
    _loc_par = prog.uniform_location("par");

    _picker = std::make_unique<pickers::BurningShip>(std::move(sp_fun));
}

} // namespace fio::fractals
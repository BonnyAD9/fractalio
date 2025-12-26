#include "powerbrot.hpp"

#include <memory>

#include "../app/maps.hpp"
#include "pickers/grid.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "powerbrot.frag"
    , 0
};

Powerbrot::Powerbrot(
    std::function<glm::mat3x2(glm::vec2)> s_fun,
    std::function<glm::mat3x2(glm::vec2)> sp_fun
) :
    IterativeFractal(FRAGMENT_SHADER, std::move(s_fun)) {
    auto &prog = program();
    prog.use();
    _loc_par = prog.uniform_location("par");
    _picker = std::make_unique<pickers::Grid>(std::move(sp_fun));
    _picker->as_fractal().map_parameter_x(0, app::maps::value(3));
    _picker->as_fractal().map_x(app::maps::value(3));
}

} // namespace fio::fractals
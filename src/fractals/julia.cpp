#include "julia.hpp"

#include <memory>

#include "pickers/mandelbrot.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "julia.frag"
    , 0
};

Julia::Julia(
    std::function<glm::mat3x2(glm::vec2)> s_fun,
    std::function<glm::mat3x2(glm::vec2)> sp_fun
) :
    ComplexFractal(FRAGMENT_SHADER, std::move(s_fun)) {
    auto &prog = program();
    prog.use();
    _loc_par = prog.uniform_location("par");

    _picker = std::make_unique<pickers::Mandelbrot>(std::move(sp_fun));
}

} // namespace fio::fractals
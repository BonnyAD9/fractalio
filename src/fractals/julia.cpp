#include "julia.hpp"

#include <memory>

#include "pickers/mandelbrot.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "julia.frag"
    , 0
};

Julia::Julia() : ComplexFractal(FRAGMENT_SHADER) {
    _picker = std::make_unique<pickers::Mandelbrot>(program());
}

} // namespace fio
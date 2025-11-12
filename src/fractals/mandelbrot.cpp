#include "mandelbrot.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "mandelbrot.frag"
    , 0
};

Mandelbrot::Mandelbrot() : ComplexFractal(FRAGMENT_SHADER) { }

} // namespace fio
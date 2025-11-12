#include "mandelbrot.hpp"

namespace fio::fractals::pickers {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "mandelbrot.frag"
    , 0
};

Mandelbrot::Mandelbrot(gl::DFShaderProgram &program) :
    Picker(FRAGMENT_SHADER, program) { }

} // namespace fio
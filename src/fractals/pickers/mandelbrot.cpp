#include "mandelbrot.hpp"

namespace fio::fractals::pickers {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "mandelbrot.frag"
    , 0
};

Mandelbrot::Mandelbrot(
    gl::DFShaderProgram &program, std::function<glm::mat3x2(glm::vec2)> s_fun
) :
    Picker(FRAGMENT_SHADER, program, std::move(s_fun)) { }

} // namespace fio::fractals::pickers
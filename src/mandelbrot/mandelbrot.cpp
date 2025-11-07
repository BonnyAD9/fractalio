#include "mandelbrot.hpp"

#include <format>

#include "../gradient.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fio {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "shader.frag"
    , 0
};

Mandelbrot::Mandelbrot() : ComplexFractal(FRAGMENT_SHADER) { }

} // namespace fio
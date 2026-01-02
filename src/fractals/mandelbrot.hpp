#pragma once

#include "../glsl/df_shader_program.hpp"
#include "iterative_fractal.hpp"

namespace fio::fractals {

class Mandelbrot : public IterativeFractal<glsl::DFShaderProgram, double> {
public:
    Mandelbrot(
        std::function<glm::mat3x2(glm::vec2)> s_fun, gl::Texture &gradient
    );

    std::string describe() override { return describe_part("Mandelbrot set"); }
};

} // namespace fio::fractals
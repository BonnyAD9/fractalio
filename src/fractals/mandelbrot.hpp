#pragma once

#include "complex_fractal.hpp"
#include "../gl/df_shader_program.hpp"

namespace fio::fractals {

class Mandelbrot : public ComplexFractal<gl::DFShaderProgram> {
public:
    Mandelbrot();

    std::string describe() override { return describe_part("Mandelbrot set"); }
};

} // namespace fio
#pragma once

#include "../gl/df_shader_program.hpp"
#include "complex_fractal.hpp"

namespace fio::fractals {

class Mandelbrot : public ComplexFractal<gl::DFShaderProgram> {
public:
    Mandelbrot();

    std::string describe() override { return describe_part("Mandelbrot set"); }
};

} // namespace fio::fractals
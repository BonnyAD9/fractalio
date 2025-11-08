#pragma once

#include "../complex_fractal/complex_fractal.hpp"
#include "../df_shader_program.hpp"

namespace fio {

class Mandelbrot : public ComplexFractal<DFShaderProgram> {
public:
    Mandelbrot();

    std::string describe() override { return describe_part("Mandelbrot set"); }
};

} // namespace fio
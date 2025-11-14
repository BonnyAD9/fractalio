#pragma once

#include "../gl/df_shader_program.hpp"
#include "complex_fractal.hpp"

namespace fio::fractals {

class BurningShip : public ComplexFractal<gl::DFShaderProgram> {
public:
    BurningShip();

    std::string describe() override { return describe_part("Burning ship"); }
};

} // namespace fio::fractals
#pragma once

#include "complex_fractal.hpp"
#include "../gl/df_shader_program.hpp"

namespace fio::fractals {

class BurningShip : public ComplexFractal<gl::DFShaderProgram> {
public:
    BurningShip();

    std::string describe() override { return describe_part("Burning ship"); }
};

} // namespace fio
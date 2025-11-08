#pragma once

#include "../complex_fractal/complex_fractal.hpp"
#include "../df_shader_program.hpp"

namespace fio {

class BurningShip : public ComplexFractal<DFShaderProgram> {
public:
    BurningShip();

    std::string describe() override { return describe_part("Burning ship"); }
};

} // namespace fio
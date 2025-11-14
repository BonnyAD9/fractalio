#pragma once

#include "../gl/df_shader_program.hpp"
#include "complex_fractal.hpp"

namespace fio::fractals {

class BurningShip : public ComplexFractal<gl::DFShaderProgram> {
public:
    BurningShip(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override { return describe_part("Burning ship"); }
};

} // namespace fio::fractals
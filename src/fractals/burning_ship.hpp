#pragma once

#include "../glsl/df_shader_program.hpp"
#include "iterative_fractal.hpp"

namespace fio::fractals {

class BurningShip : public IterativeFractal<glsl::DFShaderProgram> {
public:
    BurningShip(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override { return describe_part("Burning ship"); }
};

} // namespace fio::fractals
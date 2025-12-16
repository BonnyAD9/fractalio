#pragma once

#include "../gl/df_shader_program.hpp"
#include "complex_fractal.hpp"

namespace fio::fractals {

class Pendulum : public ComplexFractal<gl::DFShaderProgram> {
public:
    Pendulum(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override {
        return describe_part("Double pendulum");
    }
};

} // namespace fio::fractals
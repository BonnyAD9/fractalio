#pragma once

#include "../gl/framebuffer.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "chaotic_fractal.hpp"

namespace fio::fractals {

class DoublePendulum : public ChaoticFractal<gl::Program, double> {
public:
    DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override {
        return describe_part("Double pendulum");
    }
};

} // namespace fio::fractals
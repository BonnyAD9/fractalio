#pragma once

#include "../gl/framebuffer.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "complex_fractal.hpp"

namespace fio::fractals {

class DoublePendulum : public ComplexFractal<gl::Program> {
public:
    DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override {
        return describe_part("Double pendulum");
    }

    void draw(double delta) override;

private:
    gl::Texture _state;
    gl::Texture _tmp;
    gl::Framebuffer _fbuf;

    GLint _loc_step_size;
    GLint _loc_step_cnt;
    GLint _loc_action;
};

} // namespace fio::fractals
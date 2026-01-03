#pragma once

#include "../gl/program.hpp"
#include "chaotic_fractal.hpp"

namespace fio::fractals {

class ThreeBody : public ChaoticFractal<gl::Program, double> {
public:
    ThreeBody(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override { return describe_part("Three body"); }

    void drag(glm::dvec2 pos, glm::dvec2 delta) override {
        _hover = pos / size();
        _hover.y = 1 - _hover.y;
        ChaoticFractal::drag(pos, delta);
    }

protected:
    void update_parameters(bool force) override {
        auto &prog = program();
        ChaoticFractal::update_parameters(force);
        prog.uniform(_loc_hover, _hover);
    }

private:
    GLint _loc_hover;
    glm::vec2 _hover;
};

} // namespace fio::fractals
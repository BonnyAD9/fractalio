#pragma once

#include "../gl/program.hpp"
#include "chaotic_fractal.hpp"

namespace fio::fractals {

class ThreeBody : public ChaoticFractal<gl::Program, double> {
public:
    ThreeBody(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override {
        auto res = describe_part("Three body");
        res += std::format("\n gravity: {}", _g);
        res += std::format("\n mass red: {}", _m0);
        res += std::format("\n mass green: {}", _m1);
        res += std::format("\n mass blue: {}", _m2);
        res += std::format("\n space scale: {}", _sscale);
        res += std::format("\n stabilization: {}", _stabilization);
        return res;
    }

    void drag(glm::dvec2 pos, glm::dvec2 delta) override {
        _hover = pos / size();
        _hover.y = 1 - _hover.y;
        ChaoticFractal::drag(pos, delta);
    }

    void set(std::string_view param, std::optional<float> value) override;

protected:
    void update_parameters(bool force) override {
        ChaoticFractal::update_parameters(force);

        auto &prog = program();
        auto dflags = draw_flags();

        prog.uniform(_loc_hover, _hover);

        if (dflags & NEW_SPECIAL) {
            prog.uniform(_loc_g, _g);
            prog.uniform(_loc_m0, _m0);
            prog.uniform(_loc_m1, _m1);
            prog.uniform(_loc_m2, _m2);
            prog.uniform(_loc_sscale, _sscale);
            prog.uniform(_loc_stabilization, _stabilization);
        }
    }

private:
    enum DrawFlags {
        NEW_SPECIAL = NEXT_DRAW_FLAG,
    };

    GLint _loc_hover;
    glm::vec2 _hover;

    float _g = 6.67430e-11;
    GLint _loc_g;
    float _m0 = 100;
    GLint _loc_m0;
    float _m1 = 100;
    GLint _loc_m1;
    float _m2 = 100;
    GLint _loc_m2;
    float _sscale = 0.001;
    GLint _loc_sscale;
    float _stabilization = 0.00000001;
    GLint _loc_stabilization;
};

} // namespace fio::fractals
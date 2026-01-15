#pragma once

#include "../gl/program.hpp"
#include "chaotic_fractal.hpp"

namespace fio::fractals {

class DoublePendulum : public ChaoticFractal<gl::Program, double> {
public:
    DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override {
        auto res = describe_part("Double pendulum");
        res += std::format("\n  gravity: {}", _g);
        res += std::format("\n  mass 1: {}", _m1);
        res += std::format("\n  mass 2: {}", _m2);
        res += std::format("\n  length 1: {}", _l1);
        res += std::format("\n  length 2: {}", _l2);
        return res;
    }

    void set(std::string_view param, std::optional<float> value) override;

protected:
    void update_parameters(bool force) override;

private:
    enum DrawFlags {
        NEW_SPECIAL = NEXT_DRAW_FLAG,
    };

    float _g = 9.8;
    GLint _loc_g;
    float _m1 = 1;
    GLint _loc_m1;
    float _m2 = 1;
    GLint _loc_m2;
    float _l1 = 1;
    GLint _loc_l1;
    float _l2 = 1;
    GLint _loc_l2;
};

} // namespace fio::fractals
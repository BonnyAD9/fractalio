#pragma once

#include "../gl/program.hpp"
#include "chaotic_fractal.hpp"

namespace fio::fractals {

class DoublePendulum : public ChaoticFractal<gl::Program, double> {
public:
    DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override;

    void set(std::string_view param, std::optional<double> value) override;

    void save_state(std::string &out) override {
        out +=
            std::format("{}G\n", std::size_t(Fractal::Type::DOUBLE_PENDULUM));
        out += std::format(":set g 0x{:a}", _g);
        out += std::format(":set m1 0x{:a}", _m1);
        out += std::format(":set m2 0x{:a}", _m2);
        out += std::format(":set l1 0x{:a}", _l1);
        out += std::format(":set l2 0x{:a}", _l2);
        ChaoticFractal::save_state(out);
    }

    void flag(std::string_view name) override {
        if (name == "init-position") {
            set_flags(~0, flags() ^ 0x100);
        } else if (name == "pos" || name == "position") {
            set_flags(0xF, 0);
        } else if (name == "speed" || name == "velocity" || name == "vel") {
            set_flags(0xF, 1);
        } else if (name == "acc" || name == "acceleration") {
            set_flags(0xF, 2);
        } else if (name == "euler") {
            set_flags(0xF0, 0);
        } else if (name == "rk1" || name == "runge-kutta4") {
            set_flags(0xF0, 0x10);
        } else {
            ChaoticFractal::flag(name);
        }
    }

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
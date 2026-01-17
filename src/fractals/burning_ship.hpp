#pragma once

#include "../glsl/df_shader_program.hpp"
#include "iterative_fractal.hpp"

namespace fio::fractals {

class BurningShip : public IterativeFractal<glsl::DFShaderProgram, double> {
public:
    BurningShip(
        std::function<glm::mat3x2(glm::vec2)> s_fun, gl::Texture &gradient
    );

    std::string describe() override {
        auto res = describe_part("Burning ship");
        res += std::format("\n  flags: {:x}", flags());
        res += std::format("\n    coloring: {}", mb_color_flag_name(flags()));
        return res;
    }

    void save_state(std::string &out) override {
        out += std::format("{}G\n", std::size_t(Fractal::Type::BURNING_JULIA));
        IterativeFractal::save_state(out);
    }
};

} // namespace fio::fractals
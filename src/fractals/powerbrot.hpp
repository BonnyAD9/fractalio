#pragma once

#include <format>
#include <memory>

#include "../glsl/df_shader_program.hpp"
#include "iterative_fractal.hpp"
#include "pickers/picker.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

class Powerbrot : public IterativeFractal<glsl::DFShaderProgram, double> {
public:
    Powerbrot(
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        std::function<glm::mat3x2(glm::vec2)> sp_fun,
        gl::Texture &gradient
    );

    std::string describe() override {
        auto par = _picker->par();
        auto desc = describe_part("Powerbrot");
        desc += std::format(
            R".(
  power:
    {:.6} + {:.6}i
).",
            par.x,
            par.y
        );
        return desc;
    }

    Fractal *picker() override { return &_picker->as_fractal(); }

    void save_state(std::string &out) override {
        out += std::format("{}G\n", std::size_t(Fractal::Type::POWERBROT));
        IterativeFractal::save_state(out);
    }

protected:
    void update_parameters(bool force) override {
        IterativeFractal::update_parameters(force);

        if (force || _picker->new_par()) {
            program().uniform(_loc_par, _picker->par());
        }
    }

private:
    glm::ivec2 _loc_par;
    std::unique_ptr<pickers::Picker> _picker;
};

} // namespace fio::fractals
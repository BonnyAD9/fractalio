#pragma once

#include <format>
#include <memory>

#include "../glsl/df_shader_program.hpp"
#include "complex_fractal.hpp"
#include "iterative_fractal.hpp"
#include "pickers/picker.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

class Julia : public IterativeFractal<glsl::DFShaderProgram> {
public:
    Julia(
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        std::function<glm::mat3x2(glm::vec2)> sp_fun
    );

    std::string describe() override {
        auto par = _picker->par();
        auto desc = describe_part("Julia set");
        desc += std::format(
            R".(
  parameter:
    {:.6} + {:.6}i
).",
            par.x,
            par.y
        );
        return desc;
    }

    Fractal *picker() override { return &_picker->as_fractal(); }

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
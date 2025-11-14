#pragma once

#include <format>
#include <memory>

#include "../gl/df_shader_program.hpp"
#include "complex_fractal.hpp"
#include "pickers/picker.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

class Powerbrot : public ComplexFractal<gl::DFShaderProgram> {
public:
    Powerbrot();

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

    Fractal *picker() override { return _picker.get(); }

private:
    std::unique_ptr<pickers::Picker> _picker;
};

} // namespace fio::fractals
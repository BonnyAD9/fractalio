#pragma once

#include <format>
#include <memory>

#include "../gl/df_shader_program.hpp"
#include "complex_fractal.hpp"
#include "pickers/picker.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

class Julia : public ComplexFractal<gl::DFShaderProgram> {
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

    Fractal *picker() override { return _picker.get(); }

private:
    std::unique_ptr<pickers::Picker> _picker;
};

} // namespace fio::fractals
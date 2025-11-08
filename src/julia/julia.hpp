#pragma once

#include <format>
#include <memory>

#include "../complex_fractal/complex_fractal.hpp"
#include "../df_shader_program.hpp"
#include "../par_picker.hpp"

#include <glm/glm.hpp>

namespace fio {

class Julia : public ComplexFractal<DFShaderProgram> {
public:
    Julia();

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
    std::unique_ptr<ParPicker> _picker;
};

} // namespace fio
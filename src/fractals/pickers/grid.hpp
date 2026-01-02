#pragma once

#include "../../gl/program.hpp"
#include "picker.hpp"
#include "picker_impl.hpp"

namespace fio::fractals::pickers {

class Grid : public ComplexFractal<gl::Program>, public Picker {
public:
    Grid(std::function<glm::mat3x2(glm::vec2)> s_fun, gl::Texture &gradient);

    USE_PICKER_FULL(_picker, "Grid picker", _par_loc, ComplexFractal)

private:
    GLint _par_loc;

    PickerImpl _picker;
};

} // namespace fio::fractals::pickers
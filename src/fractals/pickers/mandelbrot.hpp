#pragma once

#include <functional>

#include "../../gl/program.hpp"
#include "../iterative_fractal.hpp"
#include "picker.hpp"
#include "picker_impl.hpp"

namespace fio::fractals::pickers {

class Mandelbrot : public IterativeFractal<gl::Program>, public Picker {
public:
    Mandelbrot(
        std::function<glm::mat3x2(glm::vec2)> s_fun, gl::Texture &gradient
    );

    USE_PICKER_FULL(_picker, "Mandelbrot picker", _par_loc, IterativeFractal)

private:
    GLint _par_loc;

    PickerImpl _picker;
};

} // namespace fio::fractals::pickers
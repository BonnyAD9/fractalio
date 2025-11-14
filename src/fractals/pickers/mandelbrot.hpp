#pragma once

#include "picker.hpp"

namespace fio::fractals::pickers {

class Mandelbrot : public Picker {
public:
    Mandelbrot(gl::DFShaderProgram &program);

    std::string describe() override {
        return describe_part("Mandelbrot picker");
    }
};

} // namespace fio::fractals::pickers
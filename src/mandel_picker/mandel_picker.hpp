#pragma once

#include "../par_picker.hpp"

namespace fio {

class MandelPicker : public ParPicker {
public:
    MandelPicker(DFShaderProgram &program);

    std::string describe() override {
        return describe_part("Mandelbrot picker");
    }
};

} // namespace fio
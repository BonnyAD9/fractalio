#pragma once

#include "picker.hpp"

namespace fio::fractals::pickers {

class Mandelbrot : public Picker {
public:
    Mandelbrot(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override {
        return describe_part("Mandelbrot picker");
    }
};

} // namespace fio::fractals::pickers
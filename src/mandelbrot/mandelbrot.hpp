#pragma once

#include "../complex_fractal/complex_fractal.hpp"

#include <glm/glm.hpp>

namespace fio {

class Mandelbrot : public ComplexFractal {
public:
    Mandelbrot();

    std::string describe() override { return describe_part("Mandelbrot set"); }
};

} // namespace fio
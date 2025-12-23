#pragma once

#include "../fractal.hpp"

#include <glm/glm.hpp>

namespace fio::fractals::pickers {

class Picker {
public:
    virtual bool new_par() = 0;
    virtual glm::dvec2 par() = 0;
    virtual Fractal &as_fractal() = 0;

    virtual ~Picker() = default;
};

} // namespace fio::fractals::pickers
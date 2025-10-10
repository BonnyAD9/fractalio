#pragma once

#include <glm/glm.hpp>

namespace fio {

class Fractal {
public:
    virtual void resize(glm::vec2 size) = 0;
    virtual void draw() = 0;
    virtual void use() = 0;
    virtual void drag(glm::dvec2 delta) = 0;
    virtual void scale(double delta) = 0;

    virtual ~Fractal() = default;
};

} // namespace fio
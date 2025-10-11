#pragma once

#include <functional>
#include <string>

#include <glm/glm.hpp>

namespace fio {

class Fractal {
public:
    virtual void resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of) = 0;
    virtual void draw() = 0;
    virtual void use() = 0;
    virtual void drag(glm::dvec2 delta) { (void)delta; };
    virtual void scale(double delta) { (void)delta; };
    virtual void map_iterations(const std::function<float(float)> &map) {
        (void)map;
    };

    virtual std::string describe() = 0;

    virtual ~Fractal() = default;
};

} // namespace fio
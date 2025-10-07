#pragma once

#include <span>
#include <utility>

#include <glm/glm.hpp>
namespace fio::gradient {

void linear_gradient(
    glm::u8vec3 start, glm::u8vec3 end, std::span<glm::u8vec3> dst
);

void linear_gradient(
    std::span<std::pair<float, glm::u8vec3>> pts, std::span<glm::u8vec3> dst
);

void ultra_fractal(std::span<glm::u8vec3> dst);

} // namespace fio::gradient
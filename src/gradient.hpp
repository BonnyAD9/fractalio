#pragma once

#include <span>
#include <string_view>
#include <utility>

#include <glm/glm.hpp>

namespace fio::gradient {

void linear_gradient(
    glm::u8vec3 start, glm::u8vec3 end, std::span<glm::u8vec3> dst
);

void linear_gradient(
    std::span<std::pair<float, glm::u8vec3>> pts, std::span<glm::u8vec3> dst
);

std::vector<glm::u8vec3> from_string(std::string_view gradient);

void ultra_fractal(std::span<glm::u8vec3> dst);

void grayscale(std::span<glm::u8vec3> dst);

void burn(std::span<glm::u8vec3> dst);

void monokai(std::span<glm::u8vec3> dst);

void rgb(std::span<glm::u8vec3> dst);

void cmy(std::span<glm::u8vec3> dst);

} // namespace fio::gradient
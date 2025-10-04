#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>

// make sure that the includes are not reordered
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if true
#include <GLFW/glfw3.h>
#endif

namespace fio::gl {

static inline void clear_color(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

} // namespace fio::gl
#pragma once
#include <stdexcept>

#include "gl/gl.hpp"

namespace fio::glad {

static inline void load_gl() {
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize glad.");
    }
}

} // namespace fio::glad
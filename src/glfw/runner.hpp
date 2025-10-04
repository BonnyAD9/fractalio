#pragma once

#include <functional>

#include "../gl.hpp"

namespace fio::glfw {

static constexpr int OPENGL_VERSION_MAJOR = 4;
static constexpr int OPENGL_VERSION_MINOR = 6;

static inline void run(const std::function<void()> &f) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    try {
        f();
        glfwTerminate();
        return;
    } catch (...) {
        glfwTerminate();
        throw;
    }
}

} // namespace fio::glfw
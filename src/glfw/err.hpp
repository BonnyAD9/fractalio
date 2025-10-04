#pragma once
#include <format>
#include <stdexcept>

#include "../gl.hpp"

namespace fio::glfw {

template<typename... Args>
[[noreturn]]
void throw_glfw_error(std::string_view fmt, Args &&...args) {
    std::string msg = std::vformat(fmt, std::make_format_args(args...));
    if (msg.ends_with(".")) {
        msg.pop_back();
        msg += ": ";
    }

    const char *err;
    glfwGetError(&err);
    msg += err;

    throw std::runtime_error(msg);
}

} // namespace fio::glfw
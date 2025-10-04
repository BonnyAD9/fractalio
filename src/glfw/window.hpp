#pragma once

#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <print>

#include "../gl.hpp"
#include "../make_deleter.hpp"
#include "err.hpp"

#include <glm/glm.hpp>

namespace fio::glfw {

MAKE_DELETER(GLFWwindow, glfwDestroyWindow)

class Window {
public:
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    Window(std::uint16_t width, std::uint16_t height, const char *name) :
        _window(glfwCreateWindow(width, height, name, nullptr, nullptr)) {
        if (!_window) {
            throw_glfw_error("Failed to create window.");
        }
        glfwSetWindowUserPointer(get(), reinterpret_cast<void *>(this));
        glfwSetFramebufferSizeCallback(get(), size_callback_inner);
    }

    GLFWwindow *get() { return _window.get(); }

    GLFWwindow *operator->() { return get(); }

    void make_context_current() { glfwMakeContextCurrent(get()); }

    void set_size_callback(std::function<void(int, int)> f) {
        _size_callback = std::move(f);
    }

    bool should_close() { return glfwWindowShouldClose(get()); }

    void swap_buffers() { glfwSwapBuffers(get()); }

    glm::ivec2 get_size() {
        glm::ivec2 res;
        glfwGetWindowSize(get(), &res.x, &res.y);
        return res;
    }

private:
    std::unique_ptr<GLFWwindow, del::GLFWwindow> _window;
    std::optional<std::function<void(int, int)>> _size_callback;

    static void size_callback_inner(
        GLFWwindow *window, int width, int height
    ) {
        auto win =
            reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (win->_size_callback) {
            try {
                (*win->_size_callback)(width, height);
            } catch (std::exception &ex) {
                std::println(std::cerr, "size_callback_error: {}", ex.what());
            } catch (...) {
                std::println(std::cerr, "unknown size_callback_error");
            }
        }
    }
};

} // namespace fio::glfw
#pragma once
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <print>

#include "../gl/gl.hpp"
#include "../log_err.hpp"
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
        glfwSetCursorPosCallback(get(), mouse_move_callback_inner);
        glfwSetScrollCallback(get(), scroll_callback_inner);
    }

    GLFWwindow *get() { return _window.get(); }

    GLFWwindow *operator->() { return get(); }

    void make_context_current() { glfwMakeContextCurrent(get()); }

    void set_size_callback(std::function<void(int, int)> f) {
        _size_callback = std::move(f);
    }

    void set_mouse_move_callback(std::function<void(double, double)> f) {
        _mouse_move_callback = std::move(f);
    }

    void set_scroll_callback(std::function<void(double, double)> f) {
        _scroll_callback = std::move(f);
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
    std::optional<std::function<void(double, double)>> _mouse_move_callback;
    std::optional<std::function<void(double, double)>> _scroll_callback;

    static void size_callback_inner(
        GLFWwindow *window, int width, int height
    ) {
        auto win =
            reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (win->_size_callback) {
            log_err([&]() { (*win->_size_callback)(width, height); });
        }
    }

    static void mouse_move_callback_inner(
        GLFWwindow *window, double x, double y
    ) {
        auto win =
            reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (win->_mouse_move_callback) {
            log_err([&]() { (*win->_mouse_move_callback)(x, y); });
        }
    }

    static void scroll_callback_inner(GLFWwindow *window, double x, double y) {
        auto win =
            reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (win->_mouse_move_callback) {
            log_err([&]() { (*win->_scroll_callback)(x, y); });
        }
    }
};

} // namespace fio::glfw
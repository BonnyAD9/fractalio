#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

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
        glfwSetCharCallback(get(), char_callback_inner);
        glfwSetKeyCallback(get(), key_callback_inner);
        glfwSetMouseButtonCallback(get(), mouse_press_callback_inner);
        glfwSetWindowSizeLimits(
            get(), 300, 100, GLFW_DONT_CARE, GLFW_DONT_CARE
        );
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

    void set_char_callback(std::function<void(unsigned)> f) {
        _char_callback = std::move(f);
    }

    void set_key_callback(std::function<void(int, int, int, int)> f) {
        _key_callback = std::move(f);
    }

    void set_mouse_press_callback(std::function<void(int, int, int)> f) {
        _mouse_press_callback = std::move(f);
    }

    void set_should_close(bool value) {
        glfwSetWindowShouldClose(get(), value ? GLFW_TRUE : GLFW_FALSE);
    }

    void set_clipboard_string(const char *str) {
        glfwSetClipboardString(get(), str);
    }

    const char *get_clipboard_string() {
        return glfwGetClipboardString(get());
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
    std::optional<std::function<void(unsigned)>> _char_callback;
    std::optional<std::function<void(int, int, int, int)>> _key_callback;
    std::optional<std::function<void(int, int, int)>> _mouse_press_callback;

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

    static void char_callback_inner(GLFWwindow *window, unsigned code) {
        auto win =
            reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (win->_char_callback) {
            log_err([&]() { (*win->_char_callback)(code); });
        }
    }

    static void key_callback_inner(
        GLFWwindow *window, int key, int scancode, int action, int mods
    ) {
        auto win =
            reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (win->_key_callback) {
            log_err([&]() {
                (*win->_key_callback)(key, scancode, action, mods);
            });
        }
    }

    static void mouse_press_callback_inner(
        GLFWwindow *window, int button, int action, int mods
    ) {
        auto win =
            reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (win->_char_callback) {
            log_err([&]() {
                (*win->_mouse_press_callback)(button, action, mods);
            });
        }
    }
};

} // namespace fio::glfw
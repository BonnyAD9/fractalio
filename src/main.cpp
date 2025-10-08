#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <print>
#include <string_view>

#include "fractalio.hpp"
#include "gl/gl.hpp"
#include "glad.hpp"
#include "glad/gl.h"
#include "glfw/runner.hpp"
#include "glfw/window.hpp"
#include "log_err.hpp"

namespace fio {
static void main();
}

int main() {
    if (!fio::log_err(fio::main)) {
        return EXIT_FAILURE;
    }
}

namespace fio {

constexpr int DEFAULT_WINDOW_WIDTH = 800;
constexpr int DEFAULT_WINDOW_HEIGHT = 600;

static void run();
static void err_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const char *msg,
    const void *
);

static void main() {
    glfw::run(run);
}

static void run() {
    auto window = std::make_unique<glfw::Window>(
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "fractal-io"
    );
    window->make_context_current();
    glad::load_gl();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(err_callback, nullptr);

    Fractalio app(std::move(window));
    app.mainloop();
}

static void err_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const char *msg,
    const void *
) {
    (void)source;
    (void)type;
    (void)id;
    (void)severity;

    std::string_view message{ msg, std::size_t(length) };
    std::println(std::cerr, "gl_debug: {}", message);
}

} // namespace fio

#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <print>

#include "fractalio.hpp"
#include "gl.hpp"
#include "glad.hpp"
#include "glfw/runner.hpp"
#include "glfw/window.hpp"

namespace fio {
static void main();
}

int main() {
    try {
        try {
            fio::main();
            return EXIT_SUCCESS;
        } catch (std::exception &ex) {
            std::println(std::cerr, "error: {}", ex.what());
        } catch (...) {
            std::println(std::cerr, "unknown error");
        }
        // NOLINTNEXTLINE(bugprone-empty-catch)
    } catch (...) {
        // In case the print functions also throw. Nothing can be done now.
    }
    return EXIT_FAILURE;
}

namespace fio {

constexpr int DEFAULT_WINDOW_WIDTH = 800;
constexpr int DEFAULT_WINDOW_HEIGHT = 600;

static void run();

static void main() {
    glfw::run(run);
}

static void run() {
    auto window = std::make_unique<glfw::Window>(
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "fractal-io"
    );
    window->make_context_current();
    glad::load_gl();

    Fractalio app(std::move(window));
    app.mainloop();
}

} // namespace fio

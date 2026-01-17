#include <cstdlib>
#include <iostream>
#include <memory>
#include <print>
#include <string_view>

#include "app/fractalio.hpp"
#include "cli/args.hpp"
#include "gl/gl.hpp"
#include "glad.hpp"
#include "glfw/runner.hpp"
#include "glfw/window.hpp"
#include "log_err.hpp"

namespace fio {
static void main(cli::Args args);
}

int main(int argc, char **argv) {
    if (!fio::log_err([=]() {
            fio::main(fio::cli::Args({ argv, std::size_t(argc) }));
        })) {
        return EXIT_FAILURE;
    }
}

namespace fio {

constexpr int DEFAULT_WINDOW_WIDTH = 900;
constexpr int DEFAULT_WINDOW_HEIGHT = 600;

static void run(const cli::Args &args);
static void err_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const char *msg,
    const void *
);

static void main(cli::Args args) {
    if (args.commands.empty() && args.should_exit) {
        return;
    }
    glfw::run([&]() { run(args); });
}

static void run(const cli::Args &args) {
    auto window = std::make_unique<glfw::Window>(
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "fractal-io"
    );
    window->make_context_current();
    glad::load_gl();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(err_callback, nullptr);

    app::Fractalio app(std::move(window));
    for (auto &cmd : args.commands) {
        app.run_script(cmd);
    }
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

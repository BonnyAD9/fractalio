#pragma once

#include <memory>
#include <optional>

#include "gl/buffer.hpp"
#include "gl/gl.hpp"
#include "gl/shader_program.hpp"
#include "gl/vertex_array.hpp"
#include "glfw/window.hpp"
namespace fio {

class Fractalio {
public:
    Fractalio(Fractalio &&) = delete;
    Fractalio &operator=(Fractalio &&) = delete;

    Fractalio(std::unique_ptr<glfw::Window> window);

    void mainloop();

private:
    std::unique_ptr<glfw::Window> _window;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::VertexArray _vao;
    std::optional<gl::ShaderProgram> _program;

    static void size_callback(int width, int height);
    void process_input();
};

} // namespace fio
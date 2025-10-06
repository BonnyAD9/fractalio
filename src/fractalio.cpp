#include "fractalio.hpp"

#include <memory>
#include <span>

#include "gl/gl.hpp"
#include "gl/shader.hpp"
#include "gl/shader_program.hpp"

namespace fio {

constexpr glm::vec4 DEFAULT_CLEAR_COLOR{ 0, 0, 0, 1 };

constexpr std::array VERTICES{
    -1.F, 1.F,  0.F, 1.F, 0.F, 0.F, // TR
    -1.F, -1.F, 0.F, 0.F, 1.F, 0.F, // BL
    1.F,  -1.F, 0.F, 0.F, 0.F, 1.F, // BR
    1.F,  1.F,  0.F, 0.F, 1.F, 0.F, // TR
};

constexpr std::array<GLuint, 6> INDICES{
    0, 1, 2, //
    0, 2, 3, //
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
constexpr char VERTEX_SHADER[]{
#embed "shader.vert"
    ,
    0,
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
constexpr char FRAGMENT_SHADER[]{
#embed "shader.frag"
    ,
    0,
};

Fractalio::Fractalio(std::unique_ptr<glfw::Window> window) :
    _window(std::move(window)) {
    _window->make_context_current();
    auto size = _window->get_size();
    glViewport(0, 0, size.x, size.y);
    gl::clear_color(DEFAULT_CLEAR_COLOR);
    _window->set_size_callback(size_callback);

    {
        gl::Shader vert(GL_VERTEX_SHADER);
        vert.compile(VERTEX_SHADER);
        gl::Shader frag(GL_FRAGMENT_SHADER);
        frag.compile(FRAGMENT_SHADER);
        _program.emplace();
        _program->construct(vert, frag);
    }

    constexpr GLuint LOCATION = 0;
    constexpr GLuint LOC_COLOR = 1;
    _vao.init();
    _vbo.init();
    _ebo.init();
    _vao.bind();
    _vbo.bind(GL_ARRAY_BUFFER);
    _ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, VERTICES);
    gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, INDICES);
    gl::vertex_attrib_pointer(
        LOCATION, 3, GL_FLOAT, false, 6 * sizeof(*VERTICES.data()), 0
    );
    glEnableVertexAttribArray(LOCATION);
    gl::vertex_attrib_pointer(
        LOC_COLOR,
        3,
        GL_FLOAT,
        false,
        6 * sizeof(*VERTICES.data()),
        3 * sizeof(*VERTICES.data())
    );
    glEnableVertexAttribArray(LOC_COLOR);
}

void Fractalio::mainloop() {
    _window->make_context_current();
    assert(_program);
    _program->use();
    _vao.bind();

    while (!_window->should_close()) {
        process_input();

        glClear(GL_COLOR_BUFFER_BIT);
        gl::draw_elements(GL_TRIANGLES, INDICES.size(), GL_UNSIGNED_INT, 0);

        _window->swap_buffers();
        glfwPollEvents();
    }
}

void Fractalio::size_callback(int width, int height) {
    glViewport(0, 0, width, height);
}

void Fractalio::process_input() { }

} // namespace fio
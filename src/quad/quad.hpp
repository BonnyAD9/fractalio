#pragma once

#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/program.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio {

class Quad {
public:
    Quad(glm::vec4 color);

    void resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of);

    void draw();

    void use() {
        _program.use();
        _vao.bind();
    }

private:
    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;

    GLint _loc_proj;
    GLint _loc_color;

    std::array<float, 8> _vertices{};
};

} // namespace fio
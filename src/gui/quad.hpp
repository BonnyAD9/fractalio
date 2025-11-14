#pragma once

#include <functional>

#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/program.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio::gui {

class Quad {
public:
    Quad(glm::vec4 color, std::function<glm::mat3x2(glm::vec2)> s_fun);

    void resize(glm::vec2 size);

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

    std::function<glm::mat3x2(glm::vec2)> _s_fun;

    GLint _loc_proj;
    GLint _loc_color;

    std::array<float, 8> _vertices{};
};

} // namespace fio::gui
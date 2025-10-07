#pragma once

#include <array>

#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio {

class Mandelbrot {
public:
    Mandelbrot();

    void resize(glm::vec2 size);

    void draw();

    void use() {
        _program.use();
        _vao.bind();
        _texture.bind(GL_TEXTURE_1D);
    }

private:
    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;

    std::array<float, 16> _vertices{
        -1, 1,  /* */ -2, 2,  // TL
        -1, -1, /* */ -2, -2, // BL
        1,  -1, /* */ 2,  -2, // BR,
        1,  1,  /* */ 2,  2,  // TR
    };
};

} // namespace fio
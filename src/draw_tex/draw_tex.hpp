#pragma once

#include "../fractal.hpp"
#include "../gl/buffer.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio {

class DrawTex : public Fractal {
public:
    DrawTex(const std::uint8_t *data, GLsizei w, GLsizei h, GLenum typ);

    void draw() override;

    void use() override {
        _program.use();
        _vao.bind();
        _texture.bind(GL_TEXTURE_2D);
    }

    void scale(double) override { }
    void resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of) override;

    std::string describe() override { return "Texture view"; }

private:
    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;

    GLint _loc_proj;

    std::array<float, 16> _vertices{
        0,   0,   /* */ 0, 0, // TL
        0,   600, /* */ 0, 1, // BL
        800, 600, /* */ 1, 1, // BR,
        800, 0,   /* */ 1, 0, // TR
    };
};

} // namespace fio
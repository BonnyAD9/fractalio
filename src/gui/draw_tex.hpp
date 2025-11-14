#pragma once

#include "../fractals/fractal.hpp"
#include "../gl/buffer.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio::gui {

class DrawTex : public fractals::Fractal {
public:
    DrawTex(
        const std::uint8_t *data,
        GLsizei w,
        GLsizei h,
        GLenum typ,
        std::function<glm::mat3x2(glm::vec2)> s_fun
    );

    void draw() override;

    void use() override {
        _program.use();
        _vao.bind();
        _texture.bind(GL_TEXTURE_2D);
    }

    void scale(double) override { }
    void resize(glm::vec2 size) override;

    std::string describe() override { return "Texture view"; }

private:
    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;

    std::function<glm::mat3x2(glm::vec2)> _s_fun;

    GLint _loc_proj;

    std::array<float, 16> _vertices{
        0,   0,   /* */ 0, 0, // TL
        0,   600, /* */ 0, 1, // BL
        800, 600, /* */ 1, 1, // BR,
        800, 0,   /* */ 1, 0, // TR
    };
};

} // namespace fio::gui
#pragma once

#include "../fractal.hpp"
#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
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

    void drag(glm::dvec2) override { }
    void scale(double) override { }
    void resize(glm::vec2) override { }

private:
    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;
};

} // namespace fio
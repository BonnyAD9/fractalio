#pragma once

#include <string_view>

#include "../font.hpp"
#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio {

class TextRenderer {
public:
    TextRenderer(Font &font, float line_height);

    void use() {
        _program.use();
        _vao.bind();
        _texture.bind(GL_TEXTURE_2D);
    }

    void draw() {
        gl::draw_elements(
            GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_INT, 0
        );
    }

    void resize(glm::vec2 size) const;

    void add_text(std::string_view text);

    void add_text(std::string_view text, glm::vec2 pos);

    void prepare();

    void clear_text();

private:
    Font &_font;
    std::vector<float> _vertices;
    std::vector<GLuint> _indices;

    float _line_h;
    glm::vec2 _pos{ 0, 0 };
    float _x0 = 0;

    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;
    GLint _loc_proj;
    GLint _loc_color;

    void add_glyph(Font::Glyph &glyph);
};

} // namespace fio
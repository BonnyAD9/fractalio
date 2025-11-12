#pragma once

#include <string_view>

#include "../ft/font.hpp"
#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/program.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio::gui {

class Text {
public:
    Text(ft::Font &font, float line_height);
    explicit Text(const Text &conf) :
        Text(conf._font, conf._line_h) { }

    void use() {
        _program.use();
        _vao.bind();
        _font.use();
    }

    void draw() {
        gl::draw_elements(
            GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_INT, 0
        );
    }

    void resize(glm::vec2 size) const;

    void set_transform(glm::mat4 trans) const {
        gl::uniform(_loc_trans, trans);
    }

    void set_color(glm::vec3 color) const { gl::uniform(_loc_color, color); }

    void add_text(std::string_view text);

    void add_text(std::string_view text, glm::vec2 pos);

    void prepare();

    void clear_text();

    [[nodiscard]]
    GLuint get_program() const {
        return _program.get();
    }

private:
    ft::Font &_font;
    std::vector<float> _vertices;
    std::vector<GLuint> _indices;

    float _line_h;
    glm::vec2 _pos{ 0, 0 };
    float _x0 = 0;

    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    GLint _loc_proj;
    GLint _loc_color;
    GLint _loc_trans;

    void add_glyph(ft::Font::Glyph &glyph);
};

} // namespace fio
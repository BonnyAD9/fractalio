#pragma once

#include <memory>
#include <string_view>

#include "../ft/font.hpp"
#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/program.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace fio::gui {

class Text {
public:
    Text(ft::Font &font, float line_height);
    explicit Text(const Text &conf) : Text(conf._font, conf._line_h) { }

    void draw();

    void resize(glm::vec2 size);

    void set_transform(glm::mat4 trans) { _transform = trans; }

    void set_color(glm::vec3 color) { _color = color; }

    void add_text(std::string_view text);

    void add_text(std::string_view text, glm::vec2 pos);

    void clear_text();

private:
    enum DrawFlags {
        NEW_TRIANGLES = 1,
    };

    ft::Font &_font;
    std::vector<float> _vertices;
    std::vector<GLuint> _indices;
    glm::vec2 _wsize;
    glm::mat4 _transform = glm::identity<glm::mat4>();
    glm::vec3 _color{ 1, 1, 1 };

    float _line_h;
    glm::vec2 _pos{ 0, 0 };
    float _x0 = 0;

    std::shared_ptr<gl::Program> _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    GLint _loc_proj;
    GLint _loc_color;
    GLint _loc_trans;

    int _draw_flags = 0;

    void add_glyph(ft::Font::Glyph &glyph);
    static std::shared_ptr<gl::Program> new_program();
};

} // namespace fio::gui
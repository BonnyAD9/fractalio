#include "text.hpp"

#include <memory>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fio::gui {

static constexpr GLuint LOCATION = 0;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char VERTEX_SHADER[]{
#embed "text.vert"
    , 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "text.frag"
    , 0
};

Text::Text(ft::Font &font, float line_height) :
    _font(font), _line_h(line_height), _program(new_program()) {

    _program->use();
    _loc_proj = _program->uniform_location("proj");
    _loc_color = _program->uniform_location("color");
    _loc_trans = _program->uniform_location("trans");

    _vao.bind();

    _vbo.bind(GL_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    gl::vertex_attrib_pointer(
        LOCATION, 4, GL_FLOAT, false, 4 * sizeof(float), 0
    );
    glEnableVertexAttribArray(LOCATION);

    _ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, _indices);
    glEnableVertexAttribArray(LOCATION);
}

void Text::draw() {
    if (_indices.empty()) {
        return;
    }

    _program->use();
    _vao.bind();
    _font.use();

    if (_draw_flags & NEW_TRIANGLES) {
        _vbo.bind(GL_ARRAY_BUFFER);
        gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
        glEnableVertexAttribArray(LOCATION);
        _ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
        gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, _indices);
    }

    gl::uniform(_loc_proj, glm::ortho(0.F, _wsize.x, _wsize.y, 0.F));
    gl::uniform(_loc_trans, _transform);
    gl::uniform(_loc_color, _color);

    gl::draw_elements(
        GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_INT, 0
    );

    _draw_flags = 0;
}

void Text::resize(glm::vec2 size) {
    _wsize = size;
}

void Text::add_text(std::string_view text) {
    for (auto c : text) {
        if (c == '\n') {
            _pos.x = _x0;
            _pos.y += _line_h;
            continue;
        }
        add_glyph(_font.get(c));
    }
}

void Text::add_text(std::string_view text, glm::vec2 pos) {
    _pos = pos;
    _x0 = pos.x;
    add_text(text);
}

void Text::clear_text() {
    _pos = { 0, 0 };
    _x0 = 0;
    _vertices.clear();
    _indices.clear();
}

void Text::add_glyph(ft::Font::Glyph &glyph) {
    const glm::vec2 t_tl = glm::vec2(glyph.tpos) / _font.size();
    const glm::vec2 t_br = glm::vec2(glyph.tsize) / _font.size() + t_tl;
    const glm::vec2 c_tl =
        _pos + glm::vec2({ glyph.bearing.x, -glyph.bearing.y });
    const glm::vec2 c_br = glm::vec2(glyph.tsize) + c_tl;
    _pos.x += float(glyph.advance) / 64;

    const auto i0 = GLuint(_vertices.size() / 4);

    _vertices.append_range(
        std::array{
            // clang-format off
            c_tl.x, c_tl.y, /* */ t_tl.x, t_tl.y, // TL
            c_tl.x, c_br.y, /* */ t_tl.x, t_br.y, // BL
            c_br.x, c_br.y, /* */ t_br.x, t_br.y, // BR
            c_br.x, c_tl.y, /* */ t_br.x, t_tl.y, // TR
            // clang-format on
        }
    );

    _indices.append_range(
        std::array<GLuint, 6>{
            // clang-format off
            i0 + 0, i0 + 1, i0 + 2,
            i0 + 0, i0 + 2, i0 + 3,
            // clang-format on
        }
    );

    _draw_flags |= NEW_TRIANGLES;
}

std::shared_ptr<gl::Program> Text::new_program() {
    static std::weak_ptr<gl::Program> program;

    auto res = program.lock();
    if (res) {
        return res;
    }

    auto new_program = std::make_shared<gl::Program>();
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(VERTEX_SHADER);
    gl::Shader frag(GL_FRAGMENT_SHADER);
    frag.compile(FRAGMENT_SHADER);

    new_program->link(vert, frag);

    program = new_program;
    return new_program;
}

} // namespace fio::gui
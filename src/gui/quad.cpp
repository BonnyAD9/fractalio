#include "quad.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fio::gui {

static constexpr std::array<GLuint, 6> INDICES{
    0, 1, 2, //
    0, 2, 3, //
};

static constexpr GLuint LOCATION = 0;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char VERTEX_SHADER[]{
#embed "quad.vert"
    , 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "quad.frag"
    , 0
};

Quad::Quad(glm::vec4 color) {
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(VERTEX_SHADER);
    gl::Shader frag(GL_FRAGMENT_SHADER);
    frag.compile(FRAGMENT_SHADER);

    _program.link(vert, frag);
    _program.use();

    _loc_proj = _program.uniform_location("proj");
    gl::uniform(_loc_proj, glm::ortho(0.F, 800.F, 600.F, 0.F));
    _loc_color = _program.uniform_location("color");
    gl::uniform(_loc_color, color);

    _vao.bind();

    _vbo.bind(GL_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    gl::vertex_attrib_pointer(
        LOCATION, 2, GL_FLOAT, false, 2 * sizeof(float), 0
    );
    glEnableVertexAttribArray(LOCATION);

    _ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, INDICES);
}

void Quad::resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of) {
    auto end = pos + size;

    _vertices = {
        pos.x, pos.y, pos.x, end.y, end.x, end.y, end.x, pos.y,
    };

    gl::uniform(_loc_proj, glm::ortho(0.F, of.x, of.y, 0.F));
    _vbo.bind(GL_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    glEnableVertexAttribArray(LOCATION);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Quad::draw() {
    gl::draw_elements(GL_TRIANGLES, INDICES.size(), GL_UNSIGNED_INT, 0);
}

} // namespace fio
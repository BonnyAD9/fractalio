#include "quad.hpp"

#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace fio::gui {

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

Quad::Quad(glm::vec4 color, std::function<glm::mat3x2(glm::vec2)> s_fun) :
    _s_fun(std::move(s_fun)) {
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
}

void Quad::resize(glm::vec2 size) {
    auto sizes = _s_fun(size);
    const glm::vec2 pos = sizes[0];
    _wsize = sizes[2];
    auto end = pos + sizes[1];

    _vertices = {
        pos.x, end.y, pos.x, pos.y, end.x, end.y, end.x, pos.y,
    };

    _draw_flags |= NEW_INDICES | NEW_PROJECTION;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Quad::draw() {
    _program.use();
    _vao.bind();

    if (_draw_flags & NEW_INDICES) {
        _vbo.bind(GL_ARRAY_BUFFER);
        gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    }

    if (_draw_flags & NEW_PROJECTION) {
        gl::uniform(_loc_proj, glm::ortho(0.F, _wsize.x, _wsize.y, 0.F));
    }

    _draw_flags = 0;
    gl::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
}

} // namespace fio::gui
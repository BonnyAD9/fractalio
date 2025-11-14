#include "draw_tex.hpp"

#include <array>

#include "../gl/gl.hpp"
#include "../gl/shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace fio::gui {

static constexpr std::array<GLuint, 6> INDICES{
    0, 1, 2, //
    0, 2, 3, //
};

static constexpr GLuint LOCATION = 0;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char VERTEX_SHADER[]{
#embed "draw_tex.vert"
    , 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "draw_tex.frag"
    , 0
};

DrawTex::DrawTex(
    const std::uint8_t *data,
    GLsizei w,
    GLsizei h,
    GLenum typ,
    std::function<glm::mat3x2(glm::vec2)> s_fun
) :
    _s_fun(s_fun) {
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(VERTEX_SHADER);
    gl::Shader frag(GL_FRAGMENT_SHADER);
    frag.compile(FRAGMENT_SHADER);

    _program.link(vert, frag);
    _program.use();

    _loc_proj = _program.uniform_location("proj");
    gl::uniform(_loc_proj, glm::ortho(0.F, 800.F, 600.F, 0.F));

    _texture.bind(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    std::array<float, 4> border = { 0, 0, 0, 1 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::tex_image_2d(data, w, h, typ);

    _vao.bind();

    _vbo.bind(GL_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    gl::vertex_attrib_pointer(
        LOCATION, 4, GL_FLOAT, false, 4 * sizeof(float), 0
    );
    glEnableVertexAttribArray(LOCATION);

    _ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, INDICES);
}

void DrawTex::draw() {
    gl::draw_elements(GL_TRIANGLES, INDICES.size(), GL_UNSIGNED_INT, 0);
}

void DrawTex::resize(glm::vec2 size) {
    auto sizes = _s_fun(size);
    glm::vec2 pos = sizes[0];
    glm::vec2 of = sizes[2];
    auto end = pos + sizes[1];

    _vertices = {
        pos.x, pos.y, /* */ 0, 0, // TL
        pos.x, end.y, /* */ 0, 1, // BL
        end.x, end.y, /* */ 1, 1, // BR
        end.x, pos.y, /* */ 1, 0, // TR
    };

    gl::uniform(_loc_proj, glm::ortho(0.F, of.x, of.y, 0.F));
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    glEnableVertexAttribArray(LOCATION);
}

} // namespace fio::gui
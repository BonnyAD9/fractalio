#include "draw_tex.hpp"

#include <array>

#include "../gl/gl.hpp"
#include "../gl/shader.hpp"

namespace fio {

static constexpr std::array<GLuint, 6> INDICES{
    0, 1, 2, //
    0, 2, 3, //
};

static constexpr std::array<float, 16> VERTICES{
    -1, 1,  /* */ 0, 0, // TL
    -1, -1, /* */ 0, 1, // BL
    1,  -1, /* */ 1, 1, // BR,
    1,  1,  /* */ 1, 0, // TR
};

static constexpr GLuint LOCATION = 0;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char VERTEX_SHADER[]{
#embed "shader.vert"
    , 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "shader.frag"
    , 0
};

DrawTex::DrawTex(const std::uint8_t *data, GLsizei w, GLsizei h, GLenum typ) {
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(VERTEX_SHADER);
    gl::Shader frag(GL_FRAGMENT_SHADER);
    frag.compile(FRAGMENT_SHADER);

    _program.link(vert, frag);
    _program.use();

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
    gl::buffer_data(GL_ARRAY_BUFFER, VERTICES);
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

} // namespace fio
#include "mandelbrot.hpp"

#include "../gradient.hpp"

#include <glm/fwd.hpp>

namespace fio {

static constexpr std::array<GLuint, 6> INDICES{
    0, 1, 2, //
    0, 2, 3, //
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

Mandelbrot::Mandelbrot() {
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(VERTEX_SHADER);
    gl::Shader frag(GL_FRAGMENT_SHADER);
    frag.compile(FRAGMENT_SHADER);

    _program.link(vert, frag);
    _program.use();

    _loc_center = _program.uniform_location("center");
    gl::uniform(_loc_center, _center);
    _loc_scale = _program.uniform_location("scale");
    gl::uniform(_loc_scale, _scale);

    _vao.bind();

    _vbo.bind(GL_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    gl::vertex_attrib_pointer(
        LOCATION, 4, GL_FLOAT, false, 4 * sizeof(float), 0
    );
    glEnableVertexAttribArray(LOCATION);

    _ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, INDICES);

    _texture.bind(GL_TEXTURE_1D);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::vector<glm::u8vec3> grad(256);
    gradient::ultra_fractal(grad);
    gl::tex_image_1d(grad);
}

void Mandelbrot::resize(glm::vec2 size) {
    _wsizex = size.x;

    const float h = size.y / size.x * 2;
    _vertices[3] = h;
    _vertices[7] = -h;
    _vertices[11] = -h;
    _vertices[15] = h;

    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Mandelbrot::draw() {
    gl::draw_elements(GL_TRIANGLES, INDICES.size(), GL_UNSIGNED_INT, 0);
}

void Mandelbrot::drag(glm::dvec2 delta) {
    delta.y = -delta.y;
    _center -= delta / _wsizex * 4. * _scale;
    gl::uniform(_loc_center, _center);
}

void Mandelbrot::scale(double delta) {
    _scale *= pow(0.99, -delta);
    gl::uniform(_loc_scale, _scale);
}

} // namespace fio
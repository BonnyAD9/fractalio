#include "complex_fractal.hpp"

#include <format>

#include "../gradient.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

ComplexFractal::ComplexFractal(const char *df_frag) {
    _program.compile(VERTEX_SHADER, df_frag);
    _program.use();

    _loc_proj = _program.uniform_location("proj");
    _program.uniform(_loc_proj, glm::ortho(0.F, 800.F, 600.F, 0.F));
    _loc_center = _program.uniform_location("center");
    _program.uniform(_loc_center, _center);
    _loc_scale = _program.uniform_location("scale");
    _program.uniform(_loc_scale, _scale);
    _loc_iterations = _program.uniform_location("iterations");
    _program.uniform(_loc_iterations, _iterations);
    _loc_color_count = _program.uniform_location("color_count");
    _program.uniform(_loc_color_count, _color_count);

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

void ComplexFractal::resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of) {
    _wsizex = size.x;

    auto end = pos + size;

    const float h = size.y / size.x * 2;
    _vertices = {
        pos.x, pos.y, /* */ -2, h,  // TL
        pos.x, end.y, /* */ -2, -h, // BL
        end.x, end.y, /* */ 2,  -h, // BR
        end.x, pos.y, /* */ 2,  h,  // TR
    };

    _program.uniform(_loc_proj, glm::ortho(0.F, of.x, of.y, 0.F));
    _vbo.bind(GL_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
    glEnableVertexAttribArray(LOCATION);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void ComplexFractal::draw() {
    gl::draw_elements(GL_TRIANGLES, INDICES.size(), GL_UNSIGNED_INT, 0);
}

void ComplexFractal::drag(glm::dvec2 delta) {
    delta.y = -delta.y;
    _center -= delta / _wsizex * 4. * _scale;
    _program.uniform(_loc_center, _center);
}

void ComplexFractal::scale(double delta) {
    _scale *= pow(0.99, -delta);
    _program.uniform(_loc_scale, _scale);
}

std::string ComplexFractal::describe_part(std::string_view name) {
    return std::format(
        R".(
{}
  precision: {}
  center:
    {:.6} + {:.6}i
  scale: {:.10}
  iterations: {}
  color count: {}
).",
        name,
        _program.use_double() ? "double" : "single",
        _center.x,
        _center.y,
        1 / _scale,
        _iterations,
        _color_count
    );
}

} // namespace fio
#pragma once

#include <format>
#include <string_view>

#include "../gl/buffer.hpp"
#include "../gl/texture.hpp"
#include "../gl/vertex_array.hpp"
#include "../gradient.hpp"
#include "fractal.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>

namespace fio::fractals {

namespace pickers {
class PickerImpl;
}

template<typename P> class ComplexFractal : public Fractal {
    friend pickers::PickerImpl;

public:
    ComplexFractal(
        const char *df_frag, std::function<glm::mat3x2(glm::vec2)> s_fun
    ) :
        _s_fun(std::move(s_fun)) {
        _program.compile(vertex_shader(), df_frag);
        _program.use();

        _loc_proj = _program.uniform_location("proj");
        _program.uniform(_loc_proj, glm::ortho(0.F, 800.F, 600.F, 0.F));
        _loc_center = _program.uniform_location("center");
        _program.uniform(_loc_center, _center);
        _loc_scale = _program.uniform_location("scale");
        _program.uniform(_loc_scale, _scale);

        _vao.bind();

        _vbo.bind(GL_ARRAY_BUFFER);
        gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
        gl::vertex_attrib_pointer(
            LOCATION, 4, GL_FLOAT, false, 4 * sizeof(float), 0
        );
        glEnableVertexAttribArray(LOCATION);

        _ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
        gl::buffer_data(GL_ELEMENT_ARRAY_BUFFER, indices());

        _texture.bind(GL_TEXTURE_1D);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::vector<glm::u8vec3> grad(256);
        gradient::ultra_fractal(grad);
        gl::tex_image_1d(grad);
    };

    void resize(glm::vec2 size) override {
        auto sizes = _s_fun(size);
        _size = sizes[1];
        _top_left = sizes[0];
        _wsize = sizes[2];
        const glm::vec2 pos = sizes[0];

        auto end = pos + glm::vec2(_size);

        const float h = _size.y / _size.x * 2;
        _vertices = {
            pos.x, pos.y, /* */ -2, h,  // TL
            pos.x, end.y, /* */ -2, -h, // BL
            end.x, end.y, /* */ 2,  -h, // BR
            end.x, pos.y, /* */ 2,  h,  // TR
        };

        _draw_flags |= NEW_VERTICES;
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void draw() override {
        _program.use();
        _vao.bind();
        _texture.bind(GL_TEXTURE_1D);

        if (_draw_flags & NEW_VERTICES) {
            _vbo.bind(GL_ARRAY_BUFFER);
            gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
            glEnableVertexAttribArray(LOCATION);
        }

        const bool force = _draw_flags & NEW_USE_DOUBLE;

        if (force || _draw_flags & NEW_VERTICES) {
            _program.uniform(
                _loc_proj, glm::ortho(0.F, _wsize.x, _wsize.y, 0.F)
            );
        }

        if (force || _draw_flags & NEW_CENTER) {
            _program.uniform(_loc_center, _center);
        }

        if (force || _draw_flags & NEW_SCALE) {
            _program.uniform(_loc_scale, _scale);
        }

        update_parameters(force);

        _draw_flags = 0;
        gl::draw_elements(GL_TRIANGLES, indices().size(), GL_UNSIGNED_INT, 0);
    }

    void move(glm::dvec2 delta) override {
        delta.y = -delta.y;
        _center -= delta / _size.x * 4. * _scale;
        _draw_flags |= NEW_CENTER;
    }

    void scale(double delta) override {
        _scale *= pow(0.99, -delta);
        _draw_flags |= NEW_SCALE;
    }

    double scale() override { return _scale; }
    glm::dvec2 center() { return _center; }

    void map_use_double(const std::function<bool(bool)> &map) override {
        if (_program.use_double(map(_program.use_double()))) {
            _draw_flags |= NEW_USE_DOUBLE;
        }
    }

    void map_scale(const std::function<double(double)> &map) override {
        auto s = map(_scale);
        _scale = s == 0 || std::isnan(s) ? 1 : s;
        _draw_flags |= NEW_SCALE;
    }

    void map_x(const std::function<double(double)> &map) override {
        auto x = map(_center.x);
        _center.x = std::isnan(x) ? 0 : x;
        _draw_flags |= NEW_CENTER;
    }

    void map_y(const std::function<double(double)> &map) override {
        auto y = map(_center.y);
        _center.y = std::isnan(y) ? 0 : y;
        _draw_flags |= NEW_CENTER;
    }

    void map_parameter_x(std::size_t idx, const std::function<double(double)> &map) override {
        auto p = picker();
        if (p) {
            p->map_parameter_x(idx, map);
        }
    }

    void map_parameter_y(std::size_t idx, const std::function<double(double)> &map) override {
        auto p = picker();
        if (p) {
            p->map_parameter_y(idx, map);
        }
    }

protected:
    virtual std::string describe_part(std::string_view name) {
        return std::format(
            R".(
{}
  precision: {}
  center:
    {:.6} + {:.6}i
  scale: {:.10}
).",
            name,
            _program.use_double() ? "double" : "single",
            _center.x,
            _center.y,
            1 / _scale
        );
    }

    P &program() { return _program; };

    glm::dvec2 to_part_complex(glm::dvec2 pos) {
        const double h = _size.y / _size.x * 2;
        auto norm = ((pos - _top_left) / _size - glm::dvec2{ .5, .5 }) * 2.;
        return norm * glm::dvec2{ 2, -h };
    }

    [[nodiscard]]
    constexpr double wsizex() const {
        return _size.x;
    }

    virtual void update_parameters(bool force) { (void)force; }

    enum DrawFlags {
        NEW_VERTICES = 1,
        NEW_USE_DOUBLE = NEW_VERTICES << 1,
        NEW_CENTER = NEW_USE_DOUBLE << 1,
        NEW_SCALE = NEW_CENTER << 1,
        LAST_DRAW_FLAG = NEW_SCALE,
    };

    [[nodiscard]]
    int draw_flags() const {
        return _draw_flags;
    }

    int add_draw_flag(int flag) { return _draw_flags |= flag; }

private:
    static constexpr const GLuint LOCATION = 0;

    P _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;

    std::function<glm::mat3x2(glm::vec2)> _s_fun;

    glm::dvec2 _size;
    glm::vec2 _wsize;
    glm::dvec2 _top_left;

    P::Location _loc_proj;
    glm::dvec2 _center{ 0, 0 };
    P::Location _loc_center;
    double _scale = 1.;
    P::Location _loc_scale;

    int _draw_flags = 0;

    std::array<float, 16> _vertices{
        0,   0,   /* */ -2, 2,  // TL
        0,   600, /* */ -2, -2, // BL
        800, 600, /* */ 2,  -2, // BR,
        800, 0,   /* */ 2,  2,  // TR
    };

    static constexpr const char *vertex_shader() {
        // NOLINTNEXTLINE(modernize-avoid-c-arrays)
        static constexpr char VERTEX_SHADER[]{
#embed "complex_fractal.vert"
            , 0
        };
        return VERTEX_SHADER;
    }

    static constexpr std::span<const GLuint> indices() {
        static constexpr std::array<GLuint, 6> INDICES{
            0, 1, 2, //
            0, 2, 3, //
        };
        return INDICES;
    }
};

}; // namespace fio::fractals
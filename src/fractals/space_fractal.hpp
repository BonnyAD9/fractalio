#pragma once

#include <format>

#include "../gl/buffer.hpp"
#include "../gl/vertex_array.hpp"
#include "fractal.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>

namespace fio::fractals {

namespace pickers {

class PickerImpl;

}

template<typename L> struct SpaceLocations {
    L proj;
    L center;
    L scale;
    L flags;
};

template<typename P, typename F> class SpaceFractal : public Fractal {
    friend pickers::PickerImpl;

public:
    SpaceFractal(
        P program,
        SpaceLocations<typename P::Location> loc,
        glm::vec4 space,
        std::function<glm::mat3x2(glm::vec2)> s_fun
    ) :
        _program(std::move(program)),
        _s_fun(std::move(s_fun)),
        _space(space),
        _loc(loc) {
        _vao.bind();
        _vbo.bind(GL_ARRAY_BUFFER);
        gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
        gl::vertex_attrib_pointer(0, 4, GL_FLOAT, false, 4 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
    }

    void resize(glm::vec2 size) override {
        auto sizes = _s_fun(size);
        _top_left = sizes[0];
        _size = sizes[1];
        _wsize = sizes[2];

        auto t = float(_top_left.y);
        auto l = float(_top_left.x);
        auto b = float(t + _size.y);
        auto r = float(l + _size.x);

        auto sr = glm::vec4(space_rect());
        auto la = sr.x;
        auto ra = sr.z;
        auto ta = sr.y;
        auto ba = sr.w;

        _vertices = {
            l, b, /* */ la, ba, // BL
            l, t, /* */ la, ta, // TL
            r, b, /* */ ra, ba, // BR
            r, t, /* */ ra, ta, // TR
        };

        _draw_flags |= NEW_VERTICES;
    }

    void draw(double) override {
        _program.use();
        _vao.bind();

        const bool force = _draw_flags & NEW_USE_DOUBLE;
        update_parameters(force);
        _draw_flags = 0;

        gl::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void move(glm::dvec2 delta) override {
        delta.y = -delta.y;
        _center -= delta / _size.x * (_space.z - _space.x) * double(_scale);
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

    void set_flags(GLuint mask, GLuint value) override {
        _flags &= ~mask;
        _flags |= value & mask;
        _draw_flags |= NEW_FLAGS;
    }

protected:
    enum DrawFlags {
        NEW_VERTICES = 1,
        NEW_USE_DOUBLE = NEW_VERTICES << 1,
        NEW_CENTER = NEW_USE_DOUBLE << 1,
        NEW_SCALE = NEW_CENTER << 1,
        NEW_FLAGS = NEW_SCALE << 1,

        LAST_DRAW_FLAG = NEW_FLAGS << 1,
        ALL = LAST_DRAW_FLAG - 1,
    };

    virtual std::string describe_part(std::string_view name) {
        return std::format(
            R".(
{}
  precision: {}
  flags: {:X}
  center:
    {:.6} + {:.6}i
  scale: {:.10}
).",
            name,
            _program.use_double() ? "double" : "single",
            _flags,
            _center.x,
            _center.y,
            1 / _scale
        );
    }

    P &program() { return _program; };

    [[nodiscard]]
    glm::dvec2 to_part_space(glm::dvec2 pos) const {
        pos = (pos - _top_left) / _size;
        auto sr = space_rect();
        const glm::dvec2 xy{ sr.x, sr.y };
        const glm::dvec2 zw{ sr.z, sr.w };
        return xy + pos * (zw - xy);
    }
    
    [[nodiscard]]
    glm::dvec2 to_space(glm::dvec2 pos) const {
        return (pos - glm::dvec2(_center)) / double(_scale);
    }

    [[nodiscard]]
    constexpr glm::dvec4 space_rect() const {
        const auto sh = _space.w - _space.y;
        const auto h_offset = (sh - sh * _size.y / _size.x) / 2;

        return {
            _space.x, _space.y + h_offset, _space.z, _space.w - h_offset
        };
    }

    [[nodiscard]]
    constexpr glm::dvec2 size() const {
        return _size;
    }

    [[nodiscard]]
    constexpr glm::dvec2 wsize() const {
        return _wsize;
    }

    virtual void update_parameters(bool force) {
        if (_draw_flags & NEW_VERTICES) {
            _vbo.bind(GL_ARRAY_BUFFER);
            gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
            glEnableVertexAttribArray(0);
        }

        if (force || _draw_flags & NEW_VERTICES) {
            _program.uniform(
                _loc.proj,
                glm::ortho(0.F, float(_wsize.x), float(_wsize.y), 0.F)
            );
        }

        if (force || _draw_flags & NEW_CENTER) {
            _program.uniform(_loc.center, _center);
        }

        if (force || _draw_flags & NEW_SCALE) {
            _program.uniform(_loc.scale, _scale);
        }

        if (force || _draw_flags & NEW_FLAGS) {
            _program.uniform(_loc.flags, _flags);
        }
    }

    [[nodiscard]]
    int draw_flags() const {
        return _draw_flags;
    }

    int add_draw_flag(int flag) { return _draw_flags |= flag; }

    int set_draw_flags(int flags) { return _draw_flags = flags; }

    gl::VertexArray &vao() { return _vao; }

    [[nodiscard]]
    P::Location loc_proj() const {
        return _loc.proj;
    }

private:
    P _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;

    std::function<glm::mat3x2(glm::vec2)> _s_fun;

    glm::dvec4 _space;
    glm::dvec2 _size{ 600, 600 };
    glm::dvec2 _wsize{ 800, 600 };
    glm::dvec2 _top_left{ 0, 0 };

    SpaceLocations<typename P::Location> _loc;
    glm::vec<2, F> _center{ 0, 0 };
    F _scale = 1.;
    GLuint _flags = 0;

    int _draw_flags = ALL;

    std::array<float, 16> _vertices{
        0,   600, /* */ -1, -1, // BL
        0,   0,   /* */ -1, 1,  // TL
        800, 600, /* */ 1,  -1, // BR,
        800, 0,   /* */ 1,  1,  // TR
    };
};

} // namespace fio::fractals
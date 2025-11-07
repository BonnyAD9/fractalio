#pragma once

#include <string_view>

#include "../df_shader_program.hpp"
#include "../fractal.hpp"
#include "../gl/buffer.hpp"
#include "../gl/texture.hpp"
#include "../gl/vertex_array.hpp"

namespace fio {

class ComplexFractal : public Fractal {
public:
    static constexpr GLuint DEFAULT_ITERATIONS = 256;
    static constexpr float DEFAULT_COLOR_COUNT = 256;

    ComplexFractal() = default;
    ComplexFractal(const char *df_frag);

    void resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of) override;

    void draw() override;

    void use() override {
        _program.use();
        _vao.bind();
        _texture.bind(GL_TEXTURE_1D);
    }

    void drag(glm::dvec2 delta) override;
    void scale(double delta) override;

    double scale() override { return _scale; }

    void map_iterations(const std::function<float(float)> &map) override {
        auto it = map(float(_iterations));
        _iterations =
            std::isnan(it)
                ? DEFAULT_ITERATIONS
                : GLuint(
                      std::clamp(
                          it, 0.F, float(std::numeric_limits<GLuint>::max())
                      )
                  );
        _program.uniform(_loc_iterations, _iterations);
    }

    void map_color_count(const std::function<float(float)> &map) override {
        auto cc = map(_color_count);
        if (cc == 0) {
            _color_count = 1;
        } else if (std::isnan(cc)) {
            _color_count = DEFAULT_COLOR_COUNT;
        } else {
            _color_count = cc;
        }
        _program.uniform(_loc_color_count, _color_count);
    }

    void map_use_double(const std::function<bool(bool)> &map) override {
        _program.use_double(map(_program.use_double()));
    }

    void map_scale(const std::function<double(double)> &map) override {
        auto s = map(_scale);
        _scale = s == 0 || std::isnan(s) ? 1 : s;
        _program.uniform(_loc_scale, _scale);
    }

    void map_x(const std::function<double(double)> &map) override {
        auto x = map(_center.x);
        _center.x = std::isnan(x) ? 0 : x;
        _program.uniform(_loc_center, _center);
    }

    void map_y(const std::function<double(double)> &map) override {
        auto y = map(_center.y);
        _center.y = std::isnan(y) ? 0 : y;
        _program.uniform(_loc_center, _center);
    }

protected:
    std::string describe_part(std::string_view name);

private:
    DFShaderProgram _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;

    double _wsizex = 0;

    glm::ivec2 _loc_proj;
    glm::dvec2 _center{ 0, 0 };
    glm::ivec2 _loc_center;
    double _scale = 1.;
    glm::ivec2 _loc_scale;
    GLuint _iterations = DEFAULT_ITERATIONS;
    glm::ivec2 _loc_iterations;
    float _color_count = DEFAULT_COLOR_COUNT;
    glm::ivec2 _loc_color_count;

    std::array<float, 16> _vertices{
        0,   0,   /* */ -2, 2,  // TL
        0,   600, /* */ -2, -2, // BL
        800, 600, /* */ 2,  -2, // BR,
        800, 0,   /* */ 2,  2,  // TR
    };
};

}; // namespace fio
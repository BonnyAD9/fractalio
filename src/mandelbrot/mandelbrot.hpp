#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <limits>

#include "../fractal.hpp"
#include "../gl/buffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "../gl/vertex_array.hpp"

#include <glm/glm.hpp>

namespace fio {

class Mandelbrot : public Fractal {
public:
    static constexpr GLuint DEFAULT_ITERATIONS = 256;
    static constexpr float DEFAULT_COLOR_COUNT = 256;

    Mandelbrot();

    void resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of) override;

    void draw() override;

    void use() override {
        _program.use();
        _vao.bind();
        _texture.bind(GL_TEXTURE_1D);
    }

    void drag(glm::dvec2 delta) override;
    void scale(double delta) override;

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
        gl::uniform(_loc_iterations, _iterations);
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
        gl::uniform(_loc_color_count, _color_count);
    }

    std::string describe() override;

private:
    gl::Program _program;
    gl::VertexArray _vao;
    gl::Buffer _vbo;
    gl::Buffer _ebo;
    gl::Texture _texture;

    double _wsizex = 0;

    GLint _loc_proj;
    glm::dvec2 _center{ 0, 0 };
    GLint _loc_center;
    double _scale = 1.;
    GLint _loc_scale;
    GLuint _iterations = DEFAULT_ITERATIONS;
    GLint _loc_iterations;
    float _color_count = DEFAULT_COLOR_COUNT;
    GLint _loc_color_count;

    std::array<float, 16> _vertices{
        0,   0,   /* */ -2, 2,  // TL
        0,   600, /* */ -2, -2, // BL
        800, 600, /* */ 2,  -2, // BR,
        800, 0,   /* */ 2,  2,  // TR
    };
};

} // namespace fio
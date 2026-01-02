#pragma once

#include "complex_fractal.hpp"

namespace fio::fractals {

template<typename P, typename F>
class IterativeFractal : public ComplexFractal<P, F> {
public:
    static constexpr GLuint DEFAULT_ITERATIONS = 256;
    static constexpr float DEFAULT_COLOR_COUNT = 256;

    IterativeFractal(
        const char *df_frag,
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient
    ) :
        ComplexFractal<P, F>(df_frag, std::move(s_fun), gradient) {
        auto &prog = ComplexFractal<P, F>::program();
        prog.use();

        _loc_iterations = prog.uniform_location("iterations");
        prog.uniform(_loc_iterations, _iterations);
        _loc_color_count = prog.uniform_location("color_count");
        prog.uniform(_loc_color_count, _color_count);
    }

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
        ComplexFractal<P, F>::add_draw_flag(NEW_ITERATIONS);
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
        ComplexFractal<P, F>::add_draw_flag(NEW_COLOR_COUNT);
    }

protected:
    enum DrawFlags {
        NEW_ITERATIONS = ComplexFractal<P, F>::LAST_DRAW_FLAG << 1,
        NEW_COLOR_COUNT = NEW_ITERATIONS << 1,
    };

    std::string describe_part(std::string_view name) override {
        return std::format(
            R".({}  iterations: {}
  color count: {}
).",
            ComplexFractal<P, F>::describe_part(name),
            _iterations,
            _color_count
        );
    }

    void update_parameters(bool force) override {
        ComplexFractal<P, F>::update_parameters(force);

        auto flags = ComplexFractal<P, F>::draw_flags();
        auto &prog = ComplexFractal<P, F>::program();

        if (force || flags & NEW_ITERATIONS) {
            prog.uniform(_loc_iterations, _iterations);
        }

        if (force || flags & NEW_COLOR_COUNT) {
            prog.uniform(_loc_color_count, _color_count);
        }
    }

private:
    GLuint _iterations = DEFAULT_ITERATIONS;
    P::Location _loc_iterations;
    float _color_count = DEFAULT_COLOR_COUNT;
    P::Location _loc_color_count;
};

} // namespace fio::fractals
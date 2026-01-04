#pragma once

#include "../glsl/df_shader_program.hpp"
#include "complex_fractal.hpp"
#include "pickers/picker_impl.hpp"

namespace fio::fractals {

class GravityBasins : public ComplexFractal<gl::Program, double> {
public:
    static constexpr std::size_t DEFAULT_MAX_ROOTS = 15;

    GravityBasins(
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient,
        std::size_t root_cnt = DEFAULT_MAX_ROOTS
    );

    std::string describe() override;

    USE_PICKER(_picker)

    void map_time(const std::function<double(double)> &map) override {
        _time = map(_time);
        if (std::isnan(_time)) {
            _time = 0;
        }
        ComplexFractal::add_draw_flag(NEW_TIME);
    }

    void map_step(const std::function<double(double)> &map) override {
        _step_size = map(_step_size);
        if (std::isnan(_step_size)) {
            _step_size = 0.005;
        }
        ComplexFractal::add_draw_flag(NEW_STEP_SIZE);
    }

protected:
    void update_parameters(bool force) override;

private:
    enum DrawFlags {
        NEW_TIME = SpaceFractal::NEXT_DRAW_FLAG,
        NEW_STEP_SIZE = NEW_TIME << 1,
    };

    GLint _loc_bodies;
    GLint _loc_body_cnt;
    GLint _loc_time;
    double _time = 5;
    GLint _loc_step_size;
    double _step_size = 0.005;

    pickers::PickerImpl _picker;
};

} // namespace fio::fractals
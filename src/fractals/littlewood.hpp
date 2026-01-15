#pragma once

#include "../gl/framebuffer.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "pickers/picker_impl.hpp"
#include "space_fractal.hpp"

namespace fio::fractals {

class Littlewood : public SpaceFractal<gl::Program, double> {
public:
    Littlewood(
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient,
        std::size_t max_strs = 8,
        std::size_t max_degree = 31
    );

    std::string describe() override;

    void draw(double) override;

    USE_PICKER(_picker);

protected:
    void update_parameters(bool force) override;

private:
    enum DrawFlags {
        NEW_COEF_CNT = NEXT_DRAW_FLAG,
    };

    gl::Texture &_gradient;

    gl::Program _make_histogram;
    gl::Framebuffer _fbuf;
    gl::Texture _histogram;

    GLuint _degree = 10;
    GLint _loc_degree;
    GLint _loc_store_cnt;
    GLint _loc_store;
    GLint _loc_h_aspect;
    GLint _loc_aspect;

    GLint _loc_draw_store;
    GLint _loc_draw_store_cnt;

    pickers::PickerImpl _picker;

    Littlewood(
        std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>> prog,
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient,
        std::size_t max_strs
    );
    static std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>>
    make_programs(std::size_t max_strs, std::size_t max_degree);
};

} // namespace fio::fractals
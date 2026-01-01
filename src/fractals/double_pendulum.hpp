#pragma once

#include "../gl/framebuffer.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "complex_fractal.hpp"

namespace fio::fractals {

class DoublePendulum : public ComplexFractal<gl::Program> {
public:
    DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override {
        auto res = describe_part("Double pendulum");
        res += std::format("\n  time: {}s\n", int(_time));
        return res;
    }

    void draw(double delta) override;

    void map_time(const std::function<double(double)> &map) override;

private:
    enum DrawFlags { NEW_TIME = 1 };

    gl::Texture _state;
    gl::Texture _tmp;
    gl::Framebuffer _fbuf;

    double _time;

    GLint _loc_step_size;
    GLint _loc_step_cnt;
    GLint _loc_action;
    GLint _loc_height;

    int _draw_flags = 0;
};

} // namespace fio::fractals
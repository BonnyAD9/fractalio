#pragma once

#include <cmath>

#include "../../gl/gl.hpp"
#include "../../gl/program.hpp"
#include "../complex_fractal.hpp"
#include "../fractal.hpp"

#include <glm/glm.hpp>

namespace fio::fractals::pickers {

class Picker : public ComplexFractal<gl::Program> {
public:
    Picker(const char *frag, std::function<glm::mat3x2(glm::vec2)> s_fun) :
        ComplexFractal(frag, std::move(s_fun)) {
        auto &prog = this->program();
        prog.use();
        _loc_this_par = prog.uniform_location("par");
        prog.uniform(_loc_this_par, glm::vec2(par_inv()));
    }

    [[nodiscard]]
    constexpr glm::dvec2 par() const {
        return _par;
    }

    [[nodiscard]]
    bool new_par() const {
        return _draw_flags & NEW_PAR;
    }

    constexpr void par(glm::dvec2 p) { _par = p; }

    glm::dvec2 par_inv() {
        return (_par - center()) / ComplexFractal::scale();
    }

    void drag_start(int button, int mod, glm::dvec2 pos) override {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            auto d = to_part_complex(pos) - par_inv();
            if (d.x * d.x + d.y * d.y < 0.004) {
                drag_mode(DragMode::PARAMETER);
                _x_only = (mod & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;
            } else {
                drag_mode(DragMode::MOVE);
            }
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
            drag_mode(DragMode::SCALE);
            break;
        default:
            break;
        }
    }

    void drag(glm::dvec2, glm::dvec2 delta) override {
        switch (drag_mode()) {
        case DragMode::MOVE:
            move(delta);
            break;
        case DragMode::SCALE:
            scale(delta.y);
            break;
        case DragMode::PARAMETER: {
            if (_x_only) {
                delta.y = 0;
            }
            delta.y = -delta.y;
            _par += delta / wsizex() * 4. * ComplexFractal::scale();
            auto &prog = this->program();
            prog.use();
            prog.uniform(_loc_this_par, glm::vec2(par_inv()));
            _draw_flags |= NEW_PAR;
        }
        default:
            break;
        }
    }

    std::string describe_part(std::string_view name) override {
        auto desc = ComplexFractal::describe_part(name);
        desc += std::format(
            R".(
  parameter:
    {:.6} + {:.6}i
).",
            _par.x,
            _par.y
        );
        return desc;
    }

    void map_parameter_x(const std::function<double(double)> &map) override {
        auto nx = map(_par.x);
        if (std::isnan(nx)) {
            _par.x = 0;
        } else {
            _par.x = nx;
        }
        _draw_flags |= NEW_PAR;
    }

    void map_parameter_y(const std::function<double(double)> &map) override {
        auto ny = map(_par.y);
        if (std::isnan(ny)) {
            _par.y = 0;
        } else {
            _par.y = ny;
        }
        _draw_flags |= NEW_PAR;
    }

protected:
    void update_parameters(bool force) override {
        constexpr int REDRAW_FLAGS = NEW_CENTER | NEW_SCALE;

        if (force || _draw_flags & NEW_PAR ||
            get_draw_flags() & REDRAW_FLAGS) {
            this->program().uniform(_loc_this_par, glm::vec2(par_inv()));
        }
    }

private:
    enum DrawFlags {
        NEW_PAR = 1,
    };

    int _draw_flags = NEW_PAR;
    glm::dvec2 _par{ 0, 0 };
    bool _x_only;
    GLint _loc_this_par;
};

} // namespace fio::fractals::pickers
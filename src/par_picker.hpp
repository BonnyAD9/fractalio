#pragma once

#include "complex_fractal/complex_fractal.hpp"
#include "df_shader_program.hpp"
#include "fractal.hpp"
#include "gl/program.hpp"

#include <glm/glm.hpp>

namespace fio {

class ParPicker : public ComplexFractal<gl::Program> {
public:
    ParPicker(const char *frag, DFShaderProgram &program) :
        ComplexFractal(frag), _that_program(program) {
        auto &prog = this->program();
        prog.use();
        _loc_this_par = prog.uniform_location("par");
        prog.uniform(_loc_this_par, glm::vec2(par_inv()));
        _that_program.use();
        _loc_that_par = _that_program.uniform_location("par");
        _that_program.uniform(_loc_that_par, _par);
    }

    [[nodiscard]]
    constexpr glm::vec2 par() const {
        return _par;
    }

    glm::dvec2 par_inv() {
        return (_par - center()) / ComplexFractal::scale();
    }

    void drag_start(int button, int, glm::dvec2 pos) override {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            auto d = to_part_complex(pos) - par_inv();
            if (d.x * d.x + d.y * d.y < 0.004) {
                drag_mode(DragMode::PARAMETER);
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
            use();
            move(delta);
            break;
        case DragMode::SCALE:
            use();
            scale(delta.y);
            break;
        case DragMode::PARAMETER: {
            delta.y = -delta.y;
            _par += delta / wsizex() * 4. * ComplexFractal::scale();
            auto &prog = this->program();
            prog.use();
            prog.uniform(_loc_this_par, glm::vec2(par_inv()));
            _that_program.uniform(_loc_that_par, _par);
        }
        default:
            break;
        }
    }

    void move(glm::dvec2 delta) override {
        ComplexFractal::move(delta);
        auto &prog = this->program();
        prog.use();
        prog.uniform(_loc_this_par, glm::vec2(par_inv()));
    }

    void scale(double delta) override {
        ComplexFractal::scale(delta);
        auto &prog = this->program();
        prog.use();
        prog.uniform(_loc_this_par, glm::vec2(par_inv()));
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

private:
    DFShaderProgram &_that_program;

    glm::dvec2 _par{ 0, 0 };
    GLint _loc_this_par;
    glm::ivec2 _loc_that_par;
};

} // namespace fio
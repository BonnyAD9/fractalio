#pragma once

#include <cmath>

#include "../complex_fractal.hpp"
#include "../fractal.hpp"

#include <glm/glm.hpp>

namespace fio::fractals::pickers {

class PickerImpl {
public:
    PickerImpl() = default;

    [[nodiscard]]
    constexpr glm::dvec2 par() const {
        return _par;
    }

    [[nodiscard]]
    bool new_par() const {
        return _draw_flags & NEW_PAR;
    }

    constexpr void par(glm::dvec2 p) { _par = p; }

    template<typename P> glm::dvec2 par_space(ComplexFractal<P> &frac) {
        return (_par - frac.center()) / frac.scale();
    }

    template<typename P>
    void drag_start(
        int button, int mod, glm::dvec2 pos, ComplexFractal<P> &frac
    ) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            auto d = frac.to_part_complex(pos) - par_space(frac);
            if (d.x * d.x + d.y * d.y < 0.004) {
                frac.drag_mode(DragMode::PARAMETER);
                _x_only = (mod & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;
            } else {
                frac.drag_mode(DragMode::MOVE);
            }
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
            frac.drag_mode(DragMode::SCALE);
            break;
        default:
            break;
        }
    }

    template<typename P>
    void drag(glm::dvec2, glm::dvec2 delta, ComplexFractal<P> &frac) {
        switch (frac.drag_mode()) {
        case DragMode::MOVE:
            frac.move(delta);
            break;
        case DragMode::SCALE:
            frac.scale(delta.y);
            break;
        case DragMode::PARAMETER: {
            if (_x_only) {
                delta.y = 0;
            }
            delta.y = -delta.y;
            _par += delta / frac.wsizex() * 4. * frac.scale();
            _draw_flags |= NEW_PAR;
        }
        default:
            break;
        }
    }

    template<typename P>
    std::string describe_part(std::string_view name, ComplexFractal<P> &frac) {
        auto desc = frac.describe_part(name);
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

    void map_parameter_x(const std::function<double(double)> &map) {
        auto nx = map(_par.x);
        if (std::isnan(nx)) {
            _par.x = 0;
        } else {
            _par.x = nx;
        }
        _draw_flags |= NEW_PAR;
    }

    void map_parameter_y(const std::function<double(double)> &map) {
        auto ny = map(_par.y);
        if (std::isnan(ny)) {
            _par.y = 0;
        } else {
            _par.y = ny;
        }
        _draw_flags |= NEW_PAR;
    }

    template<typename P>
    bool update_parameter(bool force, ComplexFractal<P> &frac) {
        constexpr int REDRAW_FLAGS =
            ComplexFractal<P>::NEW_CENTER | ComplexFractal<P>::NEW_SCALE;

        return force || _draw_flags & NEW_PAR ||
               frac.draw_flags() & REDRAW_FLAGS;
    }

private:
    enum DrawFlags {
        NEW_PAR = 1,
    };

    int _draw_flags = NEW_PAR;
    glm::dvec2 _par{ 0, 0 };
    bool _x_only;
};

#define USE_PICKER(picker)                                                    \
public:                                                                       \
    void drag_start(int button, int mod, glm::dvec2 pos) override {           \
        (picker).drag_start(button, mod, pos, *this);                         \
    }                                                                         \
                                                                              \
    void drag(glm::dvec2 pos, glm::dvec2 delta) override {                    \
        (picker).drag(pos, delta, *this);                                     \
    }                                                                         \
                                                                              \
    void map_parameter_x(const std::function<double(double)> &map) override { \
        (picker).map_parameter_x(map);                                        \
    }                                                                         \
                                                                              \
    void map_parameter_y(const std::function<double(double)> &map) override { \
        (picker).map_parameter_y(map);                                        \
    }                                                                         \
                                                                              \
    bool new_par() override {                                                 \
        return (picker).new_par();                                            \
    }                                                                         \
                                                                              \
    glm::dvec2 par() override {                                               \
        return (picker).par();                                                \
    }                                                                         \
                                                                              \
    Fractal &as_fractal() override {                                          \
        return *this;                                                         \
    }

#define USE_PICKER_FULL(picker, name, par_loc)                                \
    USE_PICKER(picker)                                                        \
    std::string describe() override {                                         \
        return (picker).describe_part(name, *this);                           \
    }                                                                         \
                                                                              \
protected:                                                                    \
    void update_parameters(bool force) override {                             \
        auto &prog = program();                                               \
                                                                              \
        if (_picker.update_parameter(force, *this)) {                         \
            prog.uniform(par_loc, glm::vec2((picker).par_space(*this)));      \
        }                                                                     \
    }

} // namespace fio::fractals::pickers
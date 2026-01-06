#pragma once

#include <cmath>
#include <format>

#include "../fractal.hpp"
#include "../space_fractal.hpp"

#include <glm/glm.hpp>

namespace fio::fractals::pickers {

class PickerImpl {
public:
    PickerImpl() : PickerImpl(glm::dvec2{ 0, 0 }) { }

    PickerImpl(glm::dvec2 def) : _pars({ def }) { }

    PickerImpl(std::size_t par_cnt, std::size_t max_size = 0) :
        _pars(par_cnt, { 0, 0 }), _max_size(max_size) { }

    PickerImpl(std::vector<glm::dvec2> def, std::size_t max_size = 0) :
        _pars(std::move(def)), _max_size(max_size) { }

    [[nodiscard]]
    constexpr glm::dvec2 par() const {
        return _pars[0];
    }

    [[nodiscard]]
    bool new_par() const {
        return _draw_flags & NEW_PAR;
    }
    
    void reset_flags() {
        _draw_flags = 0;
    }

    constexpr void par(glm::dvec2 p) {
        _pars[0] = p;
        _draw_flags &= NEW_PAR;
    }

    std::vector<glm::vec2> pars() {
        std::vector<glm::vec2> res;
        res.reserve(_pars.size());
        for (auto &a : _pars) {
            res.emplace_back(a);
        }
        return res;
    }

    [[nodiscard]]
    std::span<const glm::dvec2> dpars() const {
        return _pars;
    }

    [[nodiscard]]
    std::size_t max_size() const {
        return _max_size;
    }

    template<typename P, typename F>
    glm::dvec2 par_space(SpaceFractal<P, F> &frac) {
        return frac.to_space(_pars[0]);
    }

    template<typename P, typename F>
    std::vector<glm::vec2> pars_space(SpaceFractal<P, F> &frac) {
        std::vector<glm::vec2> res(_pars.size());
        for (auto &a : _pars) {
            res.emplace_back(to_space(a, frac));
        }
        return res;
    }

    template<typename P, typename F>
    void drag_start(
        int button, int mod, glm::dvec2 pos, SpaceFractal<P, F> &frac
    ) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            auto pc = frac.to_part_space(pos);
            auto close = par_below(pc, frac);
            if (!close) {
                frac.drag_mode(DragMode::MOVE);
                return;
            }
            frac.drag_mode(DragMode(int(DragMode::PARAMETER) + int(*close)));
            _x_only = (mod & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT;
            return;
        }
        case GLFW_MOUSE_BUTTON_RIGHT:
            frac.drag_mode(DragMode::SCALE);
            return;
        case GLFW_MOUSE_BUTTON_MIDDLE: {
            if (_max_size == 0) {
                return;
            }
            _draw_flags |= NEW_PAR;
            auto pc = frac.to_part_space(pos);
            auto close = par_below(pc, frac);
            if (close) {
                _pars.erase(_pars.begin() + *close);
                return;
            }
            if (_pars.size() >= _max_size) {
                return;
            }
            _pars.push_back(pc * frac.scale() + frac.center());
        }
        default:
            return;
        }
    }

    template<typename P, typename F>
    void drag(glm::dvec2, glm::dvec2 delta, SpaceFractal<P, F> &frac) {
        auto dm = frac.drag_mode();
        switch (dm) {
        case DragMode::MOVE:
            frac.move(delta);
            break;
        case DragMode::SCALE:
            frac.scale(delta.y);
            break;
        default: {
            if (int(dm) < int(DragMode::PARAMETER)) {
                break;
            }

            if (_x_only) {
                delta.y = 0;
            }
            delta.y = -delta.y;
            const std::size_t idx = int(dm) - int(DragMode::PARAMETER);
            auto sr = frac.space_rect();
            _pars[idx] += delta / (frac.size().x + frac.size().y) * 2. *
                          (sr.y - sr.x) * frac.scale();
            _draw_flags |= NEW_PAR;
        }
        }
    }

    template<typename P, typename F>
    std::string describe_part(
        std::string_view name, SpaceFractal<P, F> &frac
    ) {
        auto desc = frac.describe_part(name);
        desc += "\n  parameters:\n";
        for (auto &p : _pars) {
            desc += std::format("    {:.6} + {:.6}i\n", p.x, p.y);
        }
        return desc;
    }

    void map_parameter_x(
        std::size_t idx, const std::function<double(double)> &map
    ) {
        auto nx = map(_pars[idx].x);
        if (std::isnan(nx)) {
            _pars[idx].x = 0;
        } else {
            _pars[idx].x = nx;
        }
        _draw_flags |= NEW_PAR;
    }

    void map_parameter_y(
        std::size_t idx, const std::function<double(double)> &map
    ) {
        auto ny = map(_pars[idx].y);
        if (std::isnan(ny)) {
            _pars[idx].y = 0;
        } else {
            _pars[idx].y = ny;
        }
        _draw_flags |= NEW_PAR;
    }

    template<typename P, typename F>
    bool update_parameter(bool force, SpaceFractal<P, F> &frac) {
        constexpr int REDRAW_FLAGS =
            SpaceFractal<P, F>::NEW_CENTER | SpaceFractal<P, F>::NEW_SCALE;

        auto res =
            force || _draw_flags & NEW_PAR || frac.draw_flags() & REDRAW_FLAGS;
        _draw_flags = 0;
        return res;
    }

private:
    enum DrawFlags {
        NEW_PAR = 1,
    };

    int _draw_flags = NEW_PAR;
    std::vector<glm::dvec2> _pars;
    bool _x_only;
    std::size_t _max_size;

    template<typename P, typename F>
    std::optional<std::size_t> par_below(
        glm::dvec2 pos, SpaceFractal<P, F> &frac
    ) {
        for (std::size_t i = 0; i < _pars.size(); ++i) {
            auto d = pos - frac.to_space(_pars[i]);
            if (d.x * d.x + d.y * d.y < 0.004) {
                return i;
            }
        }
        return std::nullopt;
    }
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
    void map_parameter_x(                                                     \
        std::size_t idx, const std::function<double(double)> &map             \
    ) override {                                                              \
        (picker).map_parameter_x(idx, map);                                   \
    }                                                                         \
                                                                              \
    void map_parameter_y(                                                     \
        std::size_t idx, const std::function<double(double)> &map             \
    ) override {                                                              \
        (picker).map_parameter_y(idx, map);                                   \
    }

#define USE_PICKER_FULL(picker, name, par_loc, parent)                        \
    USE_PICKER(picker)                                                        \
    std::string describe() override {                                         \
        return (picker).describe_part(name, *this);                           \
    }                                                                         \
                                                                              \
protected:                                                                    \
    void update_parameters(bool force) override {                             \
        parent::update_parameters(force);                                     \
                                                                              \
        auto &prog = program();                                               \
                                                                              \
        if (_picker.update_parameter(force, *this)) {                         \
            prog.uniform(par_loc, glm::vec2((picker).par_space(*this)));      \
        }                                                                     \
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
    }                                                                         \
                                                                              \
    std::vector<glm::vec2> pars() override {                                  \
        return (picker).pars();                                               \
    }

} // namespace fio::fractals::pickers
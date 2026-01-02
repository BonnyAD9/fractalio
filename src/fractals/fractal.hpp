#pragma once

#include <functional>
#include <string>

#include "../gl/gl.hpp"
#include "../gui/drag_listener.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

enum class DragMode : int {
    NONE,
    MOVE,
    SCALE,
    PARAMETER,
};

class Fractal : public gui::DragListener {
public:
    enum class Type : std::size_t {
        HELP,
        MANDELBROT,
        JULIA,
        BURNING_SHIP,
        POWERBROT,
        NEWTON,
        BURNING_JULIA,
        DOUBLE_PENDULUM,
    };

    virtual void resize(glm::vec2 size) = 0;
    virtual void draw(double delta) = 0;

    void drag_start(int button, int, glm::dvec2) override {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            _drag_mode = DragMode::MOVE;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            _drag_mode = DragMode::SCALE;
            break;
        default:
            break;
        }
    }

    void drag(glm::dvec2, glm::dvec2 delta) override {
        switch (_drag_mode) {
        case DragMode::MOVE:
            move(delta);
            break;
        case DragMode::SCALE:
            scale(delta.y);
            break;
        default:
            break;
        }
    }

    void drag_end(std::optional<int> button) override {
        if (!button) {
            _drag_mode = DragMode::NONE;
            return;
        }
        switch (*button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (_drag_mode == DragMode::MOVE ||
                int(_drag_mode) >= int(DragMode::PARAMETER)) {
                _drag_mode = DragMode::NONE;
            }
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (_drag_mode == DragMode::SCALE) {
                _drag_mode = DragMode::NONE;
            }
            break;
        default:
            break;
        }
    }

    void scroll(glm::dvec2, glm::dvec2 delta) override { scale(8 * delta.y); }

    virtual void move(glm::dvec2 delta) { (void)delta; }

    virtual void scale(double delta) { (void)delta; }
    virtual double scale() { return 1; }

    virtual void map_iterations(const std::function<float(float)> &map) {
        (void)map;
    };

    virtual void map_color_count(const std::function<float(float)> &map) {
        (void)map;
    }

    virtual void map_use_double(const std::function<bool(bool)> &map) {
        (void)map;
    }

    virtual void map_scale(const std::function<double(double)> &map) {
        (void)map;
    }

    virtual void map_x(const std::function<double(double)> &map) { (void)map; }

    virtual void map_y(const std::function<double(double)> &map) { (void)map; }

    virtual void map_parameter_x(
        std::size_t idx, const std::function<double(double)> &map
    ) {
        auto p = picker();
        if (p) {
            p->map_parameter_x(idx, map);
        }
    }

    virtual void map_parameter_y(
        std::size_t idx, const std::function<double(double)> &map
    ) {
        auto p = picker();
        if (p) {
            p->map_parameter_y(idx, map);
        }
    }

    virtual void map_time(const std::function<double(double)> &map) {
        (void)map;
    }

    virtual void map_step(const std::function<double(double)> &map) {
        (void)map;
    }

    virtual void map_speed(const std::function<double(double)> &map) {
        (void)map;
    }

    virtual Fractal *picker() { return nullptr; }

    virtual std::string describe() = 0;

    virtual ~Fractal() = default;

    virtual void set_flags(GLuint mask, GLuint value) {
        (void)mask;
        (void)value;
    }

protected:
    [[nodiscard]]
    constexpr DragMode drag_mode() const {
        return _drag_mode;
    }

    constexpr void drag_mode(DragMode mode) { _drag_mode = mode; }

private:
    DragMode _drag_mode = DragMode::NONE;
};

} // namespace fio::fractals
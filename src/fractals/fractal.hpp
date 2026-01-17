#pragma once

#include <format>
#include <functional>
#include <stdexcept>
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
        THREE_BODY,
        GRAVITY_BASINS,
        LITTLEWOOD,
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

    virtual void new_gradient1() { }

    void scroll(glm::dvec2, glm::dvec2 delta) override { scale(8 * delta.y); }

    virtual void move(glm::dvec2 delta) { (void)delta; }

    virtual void scale(double delta) { (void)delta; }
    virtual double scale() { return 1; }

    virtual void set(std::string_view param, std::optional<double> value) {
        (void)value;
        throw std::runtime_error(std::format("Unknown parameter `{}`", param));
    }

    virtual void set(std::string_view param, std::optional<glm::dvec2> value) {
        (void)value;
        throw std::runtime_error(std::format("Unknown parameter `{}`", param));
    }

    virtual void flag(std::string_view name) {
        throw std::runtime_error(std::format("Unknown flag name `{}`", name));
    }

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

    std::string save_state() {
        std::string res;
        save_state(res);
        return res;
    }

    virtual void save_state(std::string &out) = 0;

protected:
    [[nodiscard]]
    constexpr DragMode drag_mode() const {
        return _drag_mode;
    }

    constexpr void drag_mode(DragMode mode) { _drag_mode = mode; }

    static bool mb_color_flag_name(std::string_view name, GLuint &flags) {
        if (name == "basic") {
            flags = (flags & ~0xF);
        } else if (name == "smooth") {
            flags = (flags & ~0xF) | 1;
        } else if (name == "shade-step") {
            flags = (flags & ~0xF) | 2;
        } else if (name == "log-smooth") {
            flags = (flags & ~0xF) | 3;
        } else if (name == "color-step") {
            flags = (flags & ~0xF) | 4;
        } else if (name == "log-shade-step") {
            flags = (flags & ~0xF) | 5;
        } else {
            return false;
        }
        return true;
    }

    static std::string_view mb_color_flag_name(GLuint flags) {
        switch (flags & 0xF) {
        default:
            return "basic";
        case 1:
            return "smooth";
        case 2:
            return "shade step";
        case 3:
            return "log smooth";
        case 4:
            return "color step";
        case 5:
            return "log shade step";
        }
    }

private:
    DragMode _drag_mode = DragMode::NONE;
};

} // namespace fio::fractals
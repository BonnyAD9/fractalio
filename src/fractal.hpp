#pragma once

#include <functional>
#include <string>

#include "drag_listener.hpp"
#include "gl/gl.hpp"

#include <glm/glm.hpp>

namespace fio {

enum class DragMode : int {
    NONE,
    MOVE,
    SCALE,
    PARAMETER,
};

class Fractal : public DragListener {
public:
    enum class Type : std::size_t {
        HELP,
        MANDELBROT,
        JULIA,
    };

    virtual void resize(glm::vec2 pos, glm::vec2 size, glm::vec2 of) = 0;
    virtual void draw() = 0;
    virtual void use() = 0;

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
            use();
            move(delta);
            break;
        case DragMode::SCALE:
            use();
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

    virtual Fractal *picker() { return nullptr; }

    virtual std::string describe() = 0;

    virtual ~Fractal() = default;

protected:
    [[nodiscard]]
    constexpr DragMode drag_mode() const {
        return _drag_mode;
    }

    constexpr void drag_mode(DragMode mode) { _drag_mode = mode; }

private:
    DragMode _drag_mode = DragMode::NONE;
};

} // namespace fio
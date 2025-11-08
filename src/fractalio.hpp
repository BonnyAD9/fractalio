#pragma once

#include <memory>
#include <unordered_map>

#include "commander.hpp"
#include "drag_listener.hpp"
#include "font.hpp"
#include "fractal.hpp"
#include "glfw/window.hpp"
#include "text_renderer/text_renderer.hpp"

namespace fio {

class Fractalio {
    friend class Commander;

public:
    Fractalio(Fractalio &&) = delete;
    Fractalio &operator=(Fractalio &&) = delete;

    Fractalio(
        std::unique_ptr<glfw::Window> window, const char *font = "monospace"
    );

    void mainloop();

private:
    static constexpr std::size_t FONT_SIZE = 16;
    static constexpr std::size_t SIDE_WIDTH = 300;
    static constexpr glm::vec4 DEFAULT_CLEAR_COLOR{ 0.1, 0.1, 0.1, 1 };

    std::unique_ptr<glfw::Window> _window;
    std::unordered_map<Fractal::Type, std::unique_ptr<Fractal>> _fractals;
    Fractal *_active = nullptr;
    Fractal *_focus = nullptr;
    glm::dvec2 _last_mouse_pos;
    Font _font;

    TextRenderer _info;
    bool _new_info = true;
    TextRenderer _fps_text;
    Commander _commander;

    glm::vec2 _wsize;

    DragListener *_drag = nullptr;

    void init_fractals();
    void size_callback(int width, int height);
    void mouse_move_callback(double x, double y);
    void mouse_press_callback(int button, int action, int mods);
    void scroll_callback(double dx, double dy);
    void key_callback(int key, int scancode, int action, int mods);
    void char_callback(unsigned code);
    void process_input();
    void consume_input();
    void execute_command(std::string_view cmd);
    void long_command(std::string_view cmd);
    void activate(Fractal::Type typ);
};

} // namespace fio
#pragma once

#include <memory>
#include <string>

#include "font.hpp"
#include "fractal.hpp"
#include "glfw/window.hpp"
#include "quad/quad.hpp"
#include "text_renderer/text_renderer.hpp"

namespace fio {

class Fractalio {
public:
    Fractalio(Fractalio &&) = delete;
    Fractalio &operator=(Fractalio &&) = delete;

    Fractalio(
        std::unique_ptr<glfw::Window> window, const char *font = "monospace"
    );

    void mainloop();

private:
    std::unique_ptr<glfw::Window> _window;
    std::unique_ptr<Fractal> _active;
    glm::dvec2 _last_mouse_pos;
    Font _font;

    TextRenderer _info;
    bool _new_info = true;
    TextRenderer _fps_text;
    TextRenderer _command_input;
    Quad _input_bg;
    std::string _input_text;
    bool _new_input;
    std::string _last_command;
    bool _no_recurse;

    glm::vec2 _wsize;

    void size_callback(int width, int height);
    void mouse_move_callback(double x, double y);
    void scroll_callback(double dx, double dy);
    void key_callback(int key, int scancode, int action, int mods);
    void char_callback(unsigned code);
    void process_input();
    void consume_input();
    void execute_command(std::string_view cmd);
    void long_command(std::string_view cmd);
};

} // namespace fio
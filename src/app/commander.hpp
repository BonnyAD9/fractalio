#pragma once

#include <string_view>

#include "../gui/quad.hpp"
#include "../gui/text.hpp"

namespace fio::app {

class Fractalio;

class Commander {
public:
    Commander(Fractalio &app, const gui::Text &text_cfg);

    void draw();
    void resize(glm::vec2 size);

    void char_in(char c);
    void consume();

    void cancel();
    void backspace() {
        if (!_text.empty()) {
            _text.pop_back();
            _draw_flags |= NEW_INPUT;
        }
    }

private:
    enum DrawFlags {
        NEW_INPUT = 1,
    };

    Fractalio &_app;
    gui::Text _input;
    gui::Quad _bg;
    std::string _text;
    std::string _last;

    glm::vec2 _wsize;
    bool _no_recurse = false;
    int _draw_flags = NEW_INPUT;

    void long_command(std::string_view cmd);
    void execute_command(std::string_view whole_cmd);
};

} // namespace fio::app
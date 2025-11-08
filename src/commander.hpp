#pragma once

#include <string_view>

#include "quad/quad.hpp"
#include "text_renderer/text_renderer.hpp"

namespace fio {

class Fractalio;

class Commander {
public:
    Commander(Fractalio &app, const TextRenderer &text_cfg);

    void prepare();
    void draw();
    void resize(glm::vec2 size);

    void char_in(char c);
    void consume();

    void cancel();
    void backspace() {
        if (!_text.empty()) {
            _text.pop_back();
            _new_input = true;
        }
    }

private:
    void long_command(std::string_view cmd);
    void execute_command(std::string_view whole_cmd);

    Fractalio &_app;
    TextRenderer _input;
    Quad _bg;
    std::string _text;
    std::string _last;

    glm::vec2 _wsize;
    bool _no_recurse = false;
    bool _new_input = true;
};

} // namespace fio
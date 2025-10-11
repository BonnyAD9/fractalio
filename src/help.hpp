#pragma once

#include "fractal.hpp"
#include "text_renderer/text_renderer.hpp"
namespace fio {

class Help : public Fractal {
public:
    Help(const TextRenderer &text);

    void resize(glm::vec2, glm::vec2, glm::vec2 of) override {
        _text.resize(of);
    }

    void draw() override { _text.draw(); }

    void use() override { _text.use(); }

    std::string describe() override { return ""; }

private:
    TextRenderer _text;
};

} // namespace fio
#pragma once

#include "fractal.hpp"
#include "../gui/text.hpp"

#include <glm/ext/matrix_transform.hpp>
namespace fio::fractals {

class Help : public Fractal {
public:
    Help(const gui::Text &text_cfg);

    void resize(glm::vec2, glm::vec2, glm::vec2 of) override {
        _text.resize(of);
    }

    void move(glm::dvec2 delta) override {
        _position += delta;
        _text.set_transform(
            glm::translate(glm::identity<glm::mat4>(), { _position, 0 })
        );
    }

    void scale(double delta) override {
        _position.y += float(delta);
        _text.set_transform(
            glm::translate(glm::identity<glm::mat4>(), { _position, 0 })
        );
    }

    void map_x(const std::function<double(double)> &map) override {
        auto x = map(_position.x);
        _position.x = std::isnan(x) ? 0 : float(x);
        _text.set_transform(
            glm::translate(glm::identity<glm::mat4>(), { _position, 0 })
        );
    }

    void map_y(const std::function<double(double)> &map) override {
        auto y = map(_position.y);
        _position.y = std::isnan(y) ? 0 : float(y);
        _text.set_transform(
            glm::translate(glm::identity<glm::mat4>(), { _position, 0 })
        );
    }

    void draw() override { _text.draw(); }

    void use() override { _text.use(); }

    std::string describe() override { return ""; }

private:
    gui::Text _text;
    glm::vec2 _position{ 0, 0 };
};

} // namespace fio
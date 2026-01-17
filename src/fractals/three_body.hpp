#pragma once

#include "../gl/program.hpp"
#include "chaotic_fractal.hpp"
#include "pickers/picker_impl.hpp"

namespace fio::fractals {

class ThreeBody : public ChaoticFractal<gl::Program, double> {
public:
    ThreeBody(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override;

    void drag(glm::dvec2 pos, glm::dvec2 delta) override {
        _hover = pos / size();
        _hover.y = 1 - _hover.y;
        _picker.drag(pos, delta, *this);
    }

    void set(std::string_view param, std::optional<double> value) override;

    void drag_start(int button, int mod, glm::dvec2 pos) override {
        _picker.drag_start(button, mod, pos, *this);
    }

    void map_parameter_x(
        std::size_t idx, const std::function<double(double)> &map
    ) override {
        _picker.map_parameter_x(idx, map);
    }

    void map_parameter_y(
        std::size_t idx, const std::function<double(double)> &map
    ) override {
        _picker.map_parameter_y(idx, map);
    }

    void save_state(std::string &out) override {
        out += std::format("{}G\n", std::size_t(Fractal::Type::THREE_BODY));
        out += std::format(":set g 0x{:a}\n", _g);
        out += std::format(":set m0 0x{:a}\n", _m0);
        out += std::format(":set m1 0x{:a}\n", _m1);
        out += std::format(":set m2 0x{:a}\n", _m2);
        out += std::format(":set sscale 0x{:a}\n", _sscale);
        out += std::format(":set stabilization 0x{:a}\n", _stabilization);
        _picker.save_state(out);
        ChaoticFractal::save_state(out);
    }

    void flag(std::string_view name) override {
        if (name == "par" || name == "overlay") {
            set_flags(~0, flags() ^ 0x200);
        } else if (name == "relative-distance" || name == "rel-dist") {
            set_flags(0xF, 0);
        } else if (name == "dist" || name == "distance") {
            set_flags(0xF, 1);
        } else if (name == "vel" || name == "velocity" || name == "speed") {
            set_flags(0xF, 2);
        } else if (name == "angle") {
            set_flags(0xF, 3);
        } else if (name == "relative-angle" || name == "rel-angle") {
            set_flags(0xF, 4);
        } else if (name == "closest-angle") {
            set_flags(0xF, 5);
        } else if (name == "euler") {
            set_flags(0xF0, 0);
        } else if (name == "rk4" || name == "runge-kutta4") {
            set_flags(0xF0, 1);
        } else if (name == "relative" || name == "rel") {
            set_flags(~0, flags() ^ 0x100);
        } else {
            ChaoticFractal::flag(name);
        }
    }

protected:
    void update_parameters(bool force) override;

private:
    enum DrawFlags {
        NEW_SPECIAL = NEXT_DRAW_FLAG,
    };

    pickers::PickerImpl _picker;

    GLint _loc_hover;
    glm::vec2 _hover;

    float _g = 6.67430e-11;
    GLint _loc_g;
    float _m0 = 100;
    GLint _loc_m0;
    float _m1 = 100;
    GLint _loc_m1;
    float _m2 = 100;
    GLint _loc_m2;
    float _sscale = 0.001;
    GLint _loc_sscale;
    float _stabilization = 0.00000001;
    GLint _loc_stabilization;
    GLint _loc_init1;
    GLint _loc_init2;
};

} // namespace fio::fractals
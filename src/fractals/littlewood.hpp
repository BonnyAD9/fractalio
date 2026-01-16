#pragma once

#include <functional>
#include <optional>

#include "../gl/framebuffer.hpp"
#include "../gl/program.hpp"
#include "../gl/texture.hpp"
#include "pickers/picker_impl.hpp"
#include "space_fractal.hpp"

namespace fio::fractals {

class Littlewood : public SpaceFractal<gl::Program, double> {
public:
    Littlewood(
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient,
        std::size_t max_strs = 8,
        std::size_t max_degree = 31
    );

    std::string describe() override;

    void draw(double) override;

    void set(std::string_view parameter, std::optional<double> value) override;

    void map_iterations(const std::function<float(float)> &map) override {
        auto it = map(float(_max_iterations));
        _max_iterations =
            std::isnan(it)
                ? 256
                : GLuint(
                      std::clamp(
                          it, 0.F, float(std::numeric_limits<GLuint>::max())
                      )
                  );
        add_draw_flag(NEW_DEGREE);
    }

    USE_PICKER(_picker);

    void save_state(std::string &out) override {
        out += std::format("{}G\n", std::size_t(Fractal::Type::LITTLEWOOD));
        out += std::format(":set degree {}\n", _degree);
        out += std::format(":set size 0x{:a}\n", _point_size);
        out += std::format(":set i {}\n", _max_iterations);
        _picker.save_state(out);
        SpaceFractal::save_state(out);
    }

    void flag(std::string_view name) override {
        if (name == "overlay" || name == "par") {
            set_flags(~0, flags() ^ 0x200);
        } else if (name == "forward") {
            set_flags(0xF, 0);
        } else if (name == "backward") {
            set_flags(0xF, 1);
        } else if (name == "abeth") {
            set_flags(0xF0, 0);
        } else if (name == "dk" || name == "durnad-kerner") {
            set_flags(0xF0, 0x10);
        } else if (name == "const" || name == "constant") {
            set_flags(~0, flags() ^ 0x100);
        } else {
            SpaceFractal::flag(name);
        }
    }

protected:
    void update_parameters(bool force) override;

private:
    enum DrawFlags {
        NEW_DEGREE = NEXT_DRAW_FLAG,
    };

    gl::Texture &_gradient;

    gl::Program _make_histogram;
    gl::Framebuffer _fbuf;
    gl::Texture _histogram;

    float _point_size = 1;
    GLuint _degree = 10;
    GLint _loc_degree;
    GLint _loc_store_cnt;
    GLint _loc_store;
    GLint _loc_h_aspect;
    GLint _loc_aspect;
    GLuint _max_iterations = 256;
    GLint _loc_max_iterations;
    GLint _loc_h_flags;

    GLint _loc_draw_store;
    GLint _loc_draw_store_cnt;

    pickers::PickerImpl _picker;

    Littlewood(
        std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>> prog,
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient,
        std::size_t max_strs
    );
    static std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>>
    make_programs(std::size_t max_strs, std::size_t max_degree);
};

} // namespace fio::fractals
#pragma once

#include <vector>

#include "../glsl/df_shader_program.hpp"
#include "iterative_fractal.hpp"
#include "pickers/picker_impl.hpp"

#include <glm/glm.hpp>

namespace fio::fractals {

class Newton : public IterativeFractal<glsl::DFShaderProgram, double> {
public:
    static constexpr std::size_t DEFAULT_MAX_ROOTS = 15;

    Newton(
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient,
        std::size_t root_cnt = DEFAULT_MAX_ROOTS
    );

    std::string describe() override;

    USE_PICKER(_picker)

    void save_state(std::string &out) override {
        out += std::format("{}G\n", std::size_t(Fractal::Type::NEWTON));
        _picker.save_state(out);
        IterativeFractal::save_state(out);
    }

    void flag(std::string_view name) override {
        if (name == "smooth") {
            set_flags(~0, flags() ^ 0x10);
        } else if (name == "flat") {
            set_flags(0xF, 0);
        } else if (name == "bright") {
            set_flags(0xF, 1);
        } else if (name == "dark") {
            set_flags(0xF, 2);
        } else if (name == "iteration" || name == "iter") {
            set_flags(0xF, 3);
        } else if (name == "log-iteration" || name == "log-iter") {
            set_flags(0xF, 4);
        } else {
            throw std::runtime_error(std::format("Unknown flag `{}`", name));
        }
    }

protected:
    void update_parameters(bool force) override;

private:
    glm::ivec2 _loc_coefs;
    glm::ivec2 _loc_roots;
    glm::ivec2 _loc_root_cnt;

    pickers::PickerImpl _picker;

    static std::vector<glm::vec2> get_coefs(std::span<glm::vec2> roots);
};

} // namespace fio::fractals
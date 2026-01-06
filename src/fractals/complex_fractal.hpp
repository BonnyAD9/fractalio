#pragma once

#include <string_view>

#include "../gl/texture.hpp"
#include "../glsl/preprocess.hpp"
#include "glad/gl.h"
#include "space_fractal.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/fwd.hpp>

namespace fio::fractals {

template<typename P, typename F>
class ComplexFractal : public SpaceFractal<P, F> {
public:
    ComplexFractal(
        std::string_view frag,
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient
    ) :
        ComplexFractal(make_program(frag), std::move(s_fun), gradient) { }

protected:
    void update_parameters(bool force) override {
        _gradient.bind(GL_TEXTURE_1D);
        SpaceFractal<P, F>::update_parameters(force);
    }

private:
    gl::Texture &_gradient;

    ComplexFractal(
        std::pair<P, SpaceLocations<typename P::Location>> prog,
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        gl::Texture &gradient
    ) :
        SpaceFractal<P, F>(
            std::move(prog.first),
            prog.second,
            { -2, 2, 2, -2 },
            std::move(s_fun)
        ),
        _gradient(gradient) { }

    static constexpr std::pair<P, SpaceLocations<typename P::Location>>
    make_program(std::string_view frag_src) {
        // NOLINTNEXTLINE(modernize-avoid-c-arrays)
        static constexpr char VERTEX_SHADER[]{
#embed "space_fractal.vert"
            , 0
        };

        P program;
        auto frag = glsl::preprocess_mylib(frag_src);
        program.compile_link(VERTEX_SHADER, frag);

        const SpaceLocations<typename P::Location> loc{
            .proj = program.uniform_location("proj"),
            .center = program.uniform_location("center"),
            .scale = program.uniform_location("scale"),
            .flags = program.uniform_location("flags"),
        };

        return { std::move(program), loc };
    }
};

}; // namespace fio::fractals
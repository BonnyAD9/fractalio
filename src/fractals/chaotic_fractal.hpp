#pragma once

#include <iostream>
#include <print>
#include <ranges>

#include "../gl/framebuffer.hpp"
#include "../gl/gl.hpp"
#include "../gl/texture.hpp"
#include "../glsl/preprocess.hpp"
#include "glad/gl.h"
#include "space_fractal.hpp"

namespace fio::fractals {

template<typename P, typename F>
class ChaoticFractal : public SpaceFractal<P, F> {
public:
    ChaoticFractal(
        std::string_view frag,
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        glm::dvec4 space = { -1, 1, 1, -1 },
        std::size_t state_cnt = 1,
        std::tuple<GLint, GLenum, GLenum> state_type = { GL_RGBA32F,
                                                         GL_RGBA,
                                                         GL_FLOAT }
    ) :
        ChaoticFractal(
            make_program(frag), std::move(s_fun), space, state_cnt, state_type
        ) {
        auto &prog = SpaceFractal<P, F>::program();
        _loc_step_cnt = prog.uniform_location("step_cnt");
        _loc_step_size = prog.uniform_location("step_size");
        _loc_action = prog.uniform_location("action");
        _loc_aspect = prog.uniform_location("aspect");

        _fbuf.bind();
        std::vector<GLenum> dbufs{ GL_NONE };
        for (GLenum i = 0; i < GLenum(state_cnt); ++i) {
            dbufs.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        gl::draw_buffers(dbufs);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        std::array<std::vector<gl::Texture> *, 2> textures{ &_states, &_tmps };
        for (auto &s :
             std::ranges::views::transform(textures, [](auto a) -> auto & {
                 return *a;
             }) | std::ranges::views::join) {
            s.bind(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
    }

    std::string describe_part(std::string_view name) override {
        auto res = SpaceFractal<P, F>::describe_part(name);
        res += std::format("  time: {}s\n", int(_time));
        res += std::format("  step: {:f}s\n", _max_step);
        res += std::format("  speed: {:.2f}s\n", _speed);
        return res;
    }

    void draw(double delta) override {
        auto &prog = SpaceFractal<P, F>::program();
        prog.use();
        SpaceFractal<P, F>::vao().bind();

        auto dflags = SpaceFractal<P, F>::draw_flags();
        const bool force = dflags & SpaceFractal<P, F>::NEW_USE_DOUBLE;
        const bool reset = dflags & (SpaceFractal<P, F>::NEW_VERTICES |
                                     SpaceFractal<P, F>::NEW_CENTER |
                                     SpaceFractal<P, F>::NEW_SCALE | NEW_TIME);

        update_parameters(force);

        const glm::vec2 fsize{ SpaceFractal<P, F>::size() };
        const glm::vec<2, GLsizei> isize{ fsize };

        GLuint action = 1; // update
        GLuint step_cnt = 1;

        if (dflags & SpaceFractal<P, F>::NEW_VERTICES) {
            std::array<std::vector<gl::Texture> *, 2> textures{ &_states,
                                                                &_tmps };
            for (auto &s :
                 std::ranges::views::transform(textures, [](auto a) -> auto & {
                     return *a;
                 }) | std::ranges::views::join) {
                s.bind(GL_TEXTURE_2D);
                gl::tex_image_2d(
                    nullptr,
                    isize.x,
                    isize.y,
                    std::get<0>(_state_type),
                    std::get<1>(_state_type),
                    std::get<2>(_state_type)
                );
            }
        }

        bind_textures();

        delta *= _speed;
        _time += delta;

        if (reset) {
            action = 0; // init
            delta = _time;
        }

        if (std::abs(delta) > std::abs(_max_step)) {
            step_cnt = std::size_t(std::abs(std::ceil(delta / _max_step)));
            delta = delta / step_cnt;
        }

        SpaceFractal<P, F>::set_draw_flags(0);

        auto lproj = SpaceFractal<P, F>::loc_proj();

        // init/update state
        glBlendFunc(GL_ONE, GL_ZERO);
        prog.uniform(_loc_action, action);
        prog.uniform(_loc_step_cnt, step_cnt);
        prog.uniform(_loc_step_size, float(delta));
        prog.uniform(lproj, glm::ortho(0.F, fsize.x, fsize.y, 0.F));
        _fbuf.bind();
        for (auto [i, t] : std::ranges::views::enumerate(_tmps)) {
            _fbuf.texture_2d(t, GL_COLOR_ATTACHMENT0 + i);
        }
        glViewport(0, 0, isize.x, isize.y);
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::println(std::cerr, "Invalid framebuffer config.");
        }
        gl::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

        const glm::vec2 fwsize{ SpaceFractal<P, F>::wsize() };
        const glm::vec<2, GLsizei> iwsize{ fwsize };

        // draw to screen
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, iwsize.x, iwsize.y);
        for (auto [s, t] : std::ranges::views::zip(_states, _tmps)) {
            std::swap(s, t);
        }
        bind_textures();
        prog.uniform(_loc_action, GLuint(2)); // draw
        prog.uniform(lproj, glm::ortho(0.F, fwsize.x, fwsize.y, 0.F));
        gl::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
        glActiveTexture(GL_TEXTURE0);
    }

    void map_time(const std::function<double(double)> &map) override {
        _time = map(_time);
        if (std::isnan(_time)) {
            _time = 0;
        }
        SpaceFractal<P, F>::add_draw_flag(NEW_TIME);
    }

    void map_step(const std::function<double(double)> &map) override {
        _max_step = map(_max_step);
        if (std::isnan(_max_step)) {
            _max_step = 0.005;
        }
    }

    void map_speed(const std::function<double(double)> &map) override {
        _speed = map(_speed);
        if (std::isnan(_speed)) {
            _speed = 1;
        }
    }

protected:
    enum DrawFlags {
        NEW_TIME = SpaceFractal<P, F>::NEXT_DRAW_FLAG,
        NEXT_DRAW_FLAG = NEW_TIME << 1,
    };

    void update_parameters(bool force) override {
        auto &prog = SpaceFractal<P, F>::program();
        auto dflags = SpaceFractal<P, F>::draw_flags();

        SpaceFractal<P, F>::update_parameters(force);

        auto s = SpaceFractal<P, F>::size();

        if (force || dflags & SpaceFractal<P, F>::NEW_VERTICES) {
            prog.uniform(_loc_aspect, float(s.y / s.x));
        }
    }

private:
    std::tuple<GLint, GLenum, GLenum> _state_type;
    std::vector<gl::Texture> _states;
    std::vector<gl::Texture> _tmps;
    gl::Framebuffer _fbuf;

    double _time = 0;
    double _max_step = 0.005;
    double _speed = 1;

    P::Location _loc_step_size;
    P::Location _loc_step_cnt;
    P::Location _loc_action;
    P::Location _loc_aspect;

    ChaoticFractal(
        std::pair<P, SpaceLocations<typename P::Location>> prog,
        std::function<glm::mat3x2(glm::vec2)> s_fun,
        glm::dvec4 space,
        std::size_t state_cnt,
        std::tuple<GLint, GLenum, GLenum> state_type
    ) :
        SpaceFractal<P, F>(
            std::move(prog.first), prog.second, space, std::move(s_fun)
        ),
        _state_type(state_type),
        _states(state_cnt),
        _tmps(state_cnt) { }

    void bind_textures() {
        for (auto [i, s] : std::ranges::views::enumerate(_states)) {
            glActiveTexture(GL_TEXTURE0 + i);
            s.bind(GL_TEXTURE_2D);
        }
    }

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

} // namespace fio::fractals
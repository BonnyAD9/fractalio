#include "double_pendulum.hpp"

#include <print>

#include "glad/gl.h"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "double_pendulum.frag"
    , 0
};

DoublePendulum::DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    ComplexFractal(FRAGMENT_SHADER, std::move(s_fun)) {
    auto &prog = program();
    _loc_step_cnt = prog.uniform_location("step_cnt");
    _loc_step_size = prog.uniform_location("step_size");
    _loc_action = prog.uniform_location("action");
    _loc_height = prog.uniform_location("height");
    
    _fbuf.bind();
    std::array<GLenum, 2> dbufs{ GL_NONE, GL_COLOR_ATTACHMENT0 };
    gl::draw_buffers(dbufs);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    for (auto s : { &_state, &_tmp }) {
        s->bind(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}

void DoublePendulum::draw(double delta) {
    auto &prog = program();
    prog.use();
    vao().bind();

    const bool force = draw_flags() & NEW_USE_DOUBLE;
    const bool reset = draw_flags() & (NEW_VERTICES | NEW_CENTER | NEW_SCALE);

    update_parameters(force);

    GLuint action = 1;
    GLuint step_cnt = 1;
    if (draw_flags() & NEW_VERTICES) {
        for (auto s : { &_tmp, &_state }) {
            s->bind(GL_TEXTURE_2D);
            gl::tex_image_2d(
                nullptr,
                GLsizei(size().x),
                GLsizei(size().y),
                GL_RGBA32F,
                GL_RGBA,
                GL_FLOAT
            );
        }
    } else {
        _state.bind(GL_TEXTURE_2D);
    }
    
    _time += delta;
    
    if (reset) {
        action = 0;
        step_cnt = 1;
        delta = _time;
    }
    
    constexpr double MAX_DELTA = 0.005;
    if (delta > MAX_DELTA) {
        step_cnt = std::size_t(delta / MAX_DELTA);
        delta = delta / step_cnt;
    }
    
    if (force || draw_flags() & NEW_VERTICES) {
        prog.uniform(_loc_height, float(size().y / size().x * 2));
    }

    set_draw_flags(0);

    // draw the state
    glBlendFunc(GL_ONE, GL_ZERO);
    prog.uniform(_loc_action, action);
    prog.uniform(_loc_step_cnt, step_cnt);
    prog.uniform(_loc_step_size, float(delta));
    prog.uniform(
        loc_proj(), glm::ortho(0.F, float(size().x), float(size().y), 0.F)
    );
    _fbuf.bind();
    _fbuf.texture_2d(_tmp, GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, GLsizei(size().x), GLsizei(size().y));
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::println("Invalid config.");
    }
    gl::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);

    // draw to screen
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, GLsizei(wsize().x), GLsizei(wsize().y));
    std::swap(_tmp, _state);
    _state.bind(GL_TEXTURE_2D);
    prog.uniform<GLuint>(_loc_action, 2);
    prog.uniform(loc_proj(), glm::ortho(0.F, wsize().x, wsize().y, 0.F));
    gl::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
}

} // namespace fio::fractals
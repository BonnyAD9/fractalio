#include "littlewood.hpp"
#include <iostream>
#include <print>
#include "space_fractal.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include "../glsl/preprocess.hpp"
#include "../gl/shader.hpp"

namespace fio::fractals {

static constexpr glm::vec4 DEFAULT_CLEAR_COLOR{ 0.1, 0.1, 0.1, 1 };
    
// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "littlewood.frag"
    , 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char VERTEX_SHADER[]{
#embed "space_fractal.vert"
, 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char VERTEX_SHADER_HISTOGRAM[]{
#embed "littlewood.vert"
, 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char GEOMETRY_SHADER[]{
#embed "littlewood.geom"
, 0
};

Littlewood::Littlewood(std::function<glm::mat3x2(glm::vec2)> s_fun, gl::Texture &gradient)
    : Littlewood(make_programs(), std::move(s_fun), gradient) {
    _loc_coef_cnt = _make_histogram.uniform_location("coef_cnt");
    _loc_store_cnt = _make_histogram.uniform_location("store_cnt");
    _loc_store = _make_histogram.uniform_location("store");
    _loc_h_aspect = _make_histogram.uniform_location("aspect");
    
    auto &prog = program();
    _loc_aspect = prog.uniform_location("aspect");
    
    _fbuf.bind();
    gl::draw_buffer(GL_COLOR_ATTACHMENT0);
    
    _histogram.bind(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::string Littlewood::describe() {
    auto res = describe_part("Littlewood");
    res += std::format("  degree: {}", _coef_cnt);
    res += "\n  coefitients:\n";
    for (auto &r : _picker.dpars()) {
        res += std::format("    {:.6} + {:.6}i\n", r.x, r.y);
    }
    return res;
}

void Littlewood::draw(double) {
    auto dflags = draw_flags();
    const bool force = dflags & NEW_USE_DOUBLE;
    bool reset = dflags & (NEW_VERTICES | NEW_CENTER | NEW_SCALE | NEW_COEF_CNT);
    reset |= _picker.new_par();
    
    glm::vec2 fsize = size();
    glm::vec<2, GLsizei> isize = fsize;
    
    if (dflags & NEW_VERTICES) {
        _histogram.bind(GL_TEXTURE_2D);
        gl::tex_image_2d(nullptr, isize.x, isize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    }
    
    if (reset) {
        glPointSize(1);
        _make_histogram.use();
        _gradient.bind(GL_TEXTURE_1D);
        vao().bind();
        glDisableVertexAttribArray(0);
        glBlendFunc(GL_ONE, GL_ONE);
        
        auto pars = _picker.pars();
        _make_histogram.uniform(_loc_coef_cnt, _coef_cnt);
        _make_histogram.uniform(_loc_store_cnt, GLuint(pars.size()));
        _make_histogram.uniform(_loc_store, pars);
        _make_histogram.uniform(_loc_h_aspect, fsize.y / fsize.x);
        //_make_histogram.uniform(loc().center, center());
        _make_histogram.uniform(loc().center, center());
        _make_histogram.uniform(loc().scale, scale());
        
        _fbuf.bind();
        _fbuf.texture_2d(_histogram);
        glViewport(0, 0, isize.x, isize.y);
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::println(std::cerr, "Invalid framebuffer config.");
        }
        gl::clear_color(glm::vec4(0, 0, 0, 1));
        glClear(GL_COLOR_BUFFER_BIT);
        gl::draw_arrays(GL_POINTS, 0, 1 << _coef_cnt);
        
        glm::vec<2, GLsizei> wisize = wsize();
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, wisize.x, wisize.y);
        glEnableVertexAttribArray(0);
        gl::clear_color(DEFAULT_CLEAR_COLOR);
    }
    
    program().use();
    vao().bind();
    _histogram.bind(GL_TEXTURE_2D);
    
    update_parameters(force);
    set_draw_flags(0);
    _picker.reset_flags();
    gl::draw_arrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Littlewood::update_parameters(bool force) {
    auto &prog = program();
    auto dflags = draw_flags();
    
    if (dflags & NEW_VERTICES) {
        vbo().bind(GL_ARRAY_BUFFER);
        gl::buffer_data(GL_ARRAY_BUFFER, vertices());
        glEnableVertexAttribArray(0);
    }
    
    if (force || dflags & NEW_VERTICES) {
        prog.uniform(loc_proj(), glm::ortho(0.F, float(wsize().x), float(wsize().y), 0.F));
        prog.uniform(_loc_aspect, float(size().y / size().x));
    }
}

Littlewood::Littlewood(std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>> prog, std::function<glm::mat3x2(glm::vec2)> s_fun, gl::Texture &gradient)
    : SpaceFractal(std::move(std::get<0>(prog)), std::get<2>(prog), {-1, 1, 1, -1}, std::move(s_fun)), _gradient(gradient), _make_histogram(std::move(std::get<1>(prog))), _picker({{-1, 0}, {1, 0}})
    {}

std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>> Littlewood::make_programs() {
    gl::Program program;
    
    std::string frag;
    glsl::preprocess_defines(frag, FRAGMENT_SHADER, { { "MODE", "1" } });
    frag = glsl::preprocess_mylib(frag);
    
    program.compile_link(VERTEX_SHADER, frag);
    
    gl::Program make_histogram;
    
    frag.clear();
    glsl::preprocess_defines(frag, FRAGMENT_SHADER, { { "MODE", "0" } });
    frag = glsl::preprocess_mylib(frag);
    
    gl::Shader hvert(GL_VERTEX_SHADER);
    hvert.compile(VERTEX_SHADER_HISTOGRAM);
    
    gl::Shader hgeom(GL_GEOMETRY_SHADER);
    hgeom.compile(glsl::preprocess_mylib(GEOMETRY_SHADER));
    
    gl::Shader hfrag(GL_FRAGMENT_SHADER);
    hfrag.compile(frag);
    
    make_histogram.attach(hvert);
    make_histogram.attach(hgeom);
    make_histogram.attach(hfrag);
    make_histogram.link();
    
    const SpaceLocations<GLint> loc{
        .proj = program.uniform_location("proj"),
        .center = make_histogram.uniform_location("center"),
        .scale = make_histogram.uniform_location("scale"),
        .flags = 0,
    };
    
    return { std::move(program), std::move(make_histogram), loc };
}
    
}
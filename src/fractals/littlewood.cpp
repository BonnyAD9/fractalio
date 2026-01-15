#include "littlewood.hpp"

#include <format>
#include <iostream>
#include <print>
#include <unordered_map>

#include "../gl/shader.hpp"
#include "../glsl/preprocess.hpp"
#include "space_fractal.hpp"

#include <glm/ext/matrix_clip_space.hpp>

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

Littlewood::Littlewood(
    std::function<glm::mat3x2(glm::vec2)> s_fun,
    gl::Texture &gradient,
    std::size_t max_strs,
    std::size_t max_degree
) :
    Littlewood(
        make_programs(max_strs, max_degree),
        std::move(s_fun),
        gradient,
        max_strs
    ) {
    _loc_degree = _make_histogram.uniform_location("degree");
    _loc_store_cnt = _make_histogram.uniform_location("store_cnt");
    _loc_store = _make_histogram.uniform_location("store");
    _loc_h_aspect = _make_histogram.uniform_location("aspect");

    auto &prog = program();
    _loc_aspect = prog.uniform_location("aspect");
    _loc_draw_store = prog.uniform_location("store");
    _loc_draw_store_cnt = prog.uniform_location("store_cnt");

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
    res += std::format("  degree: {}", _degree);
    res += "\n  coefitients:\n";
    for (auto &r : _picker.dpars()) {
        res += std::format("    {:.6} + {:.6}i\n", r.x, r.y);
    }
    return res;
}

void Littlewood::draw(double) {
    auto dflags = draw_flags();
    const bool force = dflags & NEW_USE_DOUBLE;
    bool reset = dflags & (NEW_VERTICES | NEW_CENTER | NEW_SCALE | NEW_DEGREE);
    reset |= _picker.new_par();

    const glm::vec2 fsize = size();
    const glm::vec<2, GLsizei> isize = fsize;

    if (dflags & NEW_VERTICES) {
        _histogram.bind(GL_TEXTURE_2D);
        gl::tex_image_2d(
            nullptr, isize.x, isize.y, GL_RGBA32F, GL_RGBA, GL_FLOAT
        );
    }

    if (reset) {
        glPointSize(1);
        _make_histogram.use();
        _gradient.bind(GL_TEXTURE_1D);
        vao().bind();
        glDisableVertexAttribArray(0);
        glBlendFunc(GL_ONE, GL_ONE);

        auto pars = _picker.pars();
        _make_histogram.uniform(_loc_degree, _degree);
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
        gl::draw_arrays(GL_POINTS, 0, GLsizei(std::pow(pars.size(), _degree)));

        const glm::vec<2, GLsizei> wisize = wsize();

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

void Littlewood::set(std::string_view parameter, std::optional<float> value) {
    if (parameter == "degree") {
        _degree = GLuint(value.value_or(10));
        add_draw_flag(NEW_DEGREE);
    }
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
        prog.uniform(
            loc_proj(),
            glm::ortho(0.F, float(wsize().x), float(wsize().y), 0.F)
        );
        prog.uniform(_loc_aspect, float(size().y / size().x));
    }

    if (dflags & NEW_FLAGS) {
        prog.uniform(loc().flags, flags());
    }

    if (_picker.new_par() || dflags & NEW_SCALE || dflags & NEW_CENTER) {
        auto dstore = _picker.dpars();
        prog.uniform(_loc_draw_store_cnt, GLuint(dstore.size()));

        std::vector<glm::vec2> store(dstore.size());
        for (std::size_t i = 0; i < dstore.size(); ++i) {
            store[i] = glm::vec2((dstore[i] - center()) / scale());
        }
        prog.uniform(_loc_draw_store, store);
    }
}

Littlewood::Littlewood(
    std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>> prog,
    std::function<glm::mat3x2(glm::vec2)> s_fun,
    gl::Texture &gradient,
    std::size_t max_strs
) :
    SpaceFractal(
        std::move(std::get<0>(prog)),
        std::get<2>(prog),
        { -1, 1, 1, -1 },
        std::move(s_fun)
    ),
    _gradient(gradient),
    _make_histogram(std::move(std::get<1>(prog))),
    _picker({ { -1, 0 }, { 1, 0 } }, max_strs) { }

std::tuple<gl::Program, gl::Program, SpaceLocations<GLint>>
Littlewood::make_programs(std::size_t max_strs, std::size_t max_degree) {
    gl::Program program;

    std::string smax_store = std::format("{}", max_strs);
    std::string smax_degree = std::format("{}", max_degree);

    std::string shader;
    glsl::preprocess_defines(
        shader,
        FRAGMENT_SHADER,
        { { "MODE", "1" }, { "MAX_STORE", smax_store } }
    );

    program.compile_link(VERTEX_SHADER, glsl::preprocess_mylib(shader));

    gl::Program make_histogram;

    gl::Shader hvert(GL_VERTEX_SHADER);
    hvert.compile(VERTEX_SHADER_HISTOGRAM);

    gl::Shader hgeom(GL_GEOMETRY_SHADER);
    shader.clear();
    glsl::preprocess_defines(
        shader,
        GEOMETRY_SHADER,
        { { "MAX_STORE", smax_store }, { "MAX_DEGREE", smax_degree } }
    );
    hgeom.compile(glsl::preprocess_mylib(shader));

    shader.clear();
    glsl::preprocess_defines(shader, FRAGMENT_SHADER, { { "MODE", "0" } });
    gl::Shader hfrag(GL_FRAGMENT_SHADER);
    hfrag.compile(glsl::preprocess_mylib(shader));

    make_histogram.attach(hvert);
    make_histogram.attach(hgeom);
    make_histogram.attach(hfrag);
    make_histogram.link();

    const SpaceLocations<GLint> loc{
        .proj = program.uniform_location("proj"),
        .center = make_histogram.uniform_location("center"),
        .scale = make_histogram.uniform_location("scale"),
        .flags = program.uniform_location("flags"),
    };

    return { std::move(program), std::move(make_histogram), loc };
}

} // namespace fio::fractals
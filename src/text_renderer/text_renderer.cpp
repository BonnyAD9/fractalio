#include "text_renderer.hpp"
#include "glad/gl.h"
#include <glm/fwd.hpp>

namespace fio {

static constexpr GLuint LOCATION = 0;

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char VERTEX_SHADER[]{
#embed "shader.vert"
    , 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "shader.frag"
    , 0
};
    
TextRenderer::TextRenderer(Font font, float line_height) : _font(std::move(font)), _line_h(line_height) {
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(VERTEX_SHADER);
    gl::Shader frag(GL_FRAGMENT_SHADER);
    frag.compile(FRAGMENT_SHADER);
    
    _program.link(vert, frag);
    _program.use();
    
    _loc_proj = _program.uniform_location("proj");
    gl::uniform(_loc_proj, glm::mat4());
    _loc_color = _program.uniform_location("color");
    gl::uniform(_loc_color, glm::vec3{ 1, 1, 1 });
    
    _vao.bind();
    
    _vbo.bind(GL_ARRAY_BUFFER);
    gl::buffer_data(GL_ARRAY_BUFFER, _vertices);
}
    
}
#include "df_shader_program.hpp"

#include <unordered_map>

#include "../gl/shader.hpp"
#include "preprocess.hpp"

namespace fio::glsl {

void DFShaderProgram::compile_link(
    const char *vert_src, std::string_view frag_src
) {
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(vert_src);

    std::string src;
    preprocess_defines(src, frag_src, { { "PRECISION", "4" } });
    gl::Shader float_frag(GL_FRAGMENT_SHADER);
    float_frag.compile(src);

    src.clear();
    preprocess_defines(src, frag_src, { { "PRECISION", "8" } });
    gl::Shader double_frag(GL_FRAGMENT_SHADER);
    double_frag.compile(src);

    _float.link(vert, float_frag);
    _double.link(vert, double_frag);
}

} // namespace fio::glsl
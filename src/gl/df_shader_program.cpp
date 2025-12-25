#include "df_shader_program.hpp"

#include <unordered_map>

#include "process_defines.hpp"
#include "shader.hpp"

namespace fio::gl {

void DFShaderProgram::compile(
    const char *vert_src, std::string_view frag_src
) {
    gl::Shader vert(GL_VERTEX_SHADER);
    vert.compile(vert_src);

    std::string src;
    process_defines(
        src,
        frag_src,
        {
            { "ploat", "float" },
            { "pvec2", "vec2" },
            { "patan", "atan" },
            { "pcos", "cos" },
            { "psin", "sin" },
            { "pexp", "exp" },
            { "plog", "log" },
            { "ppow", "pow" },
        }
    );
    gl::Shader float_frag(GL_FRAGMENT_SHADER);
    float_frag.compile(src.c_str());

    src.clear();
    process_defines(
        src,
        frag_src,
        {
            { "ploat", "double" },
            { "pvec2", "dvec2" },
            { "patan", "datan" },
            { "pcos", "dcos" },
            { "psin", "dsin" },
            { "pexp", "dexp" },
            { "plog", "dlog" },
            { "ppow", "dpow" },
        }
    );
    gl::Shader double_frag(GL_FRAGMENT_SHADER);
    double_frag.compile(src.c_str());

    _float.link(vert, float_frag);
    _double.link(vert, double_frag);
}

} // namespace fio::gl
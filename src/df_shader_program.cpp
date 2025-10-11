#include "df_shader_program.hpp"

#include <ranges>
#include <unordered_map>

#include "gl/shader.hpp"

namespace fio {

static void process_defines(
    std::string &out,
    std::string_view src,
    const std::unordered_map<std::string_view, std::string_view> &def_map
);

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
        }
    );
    gl::Shader double_frag(GL_FRAGMENT_SHADER);
    double_frag.compile(src.c_str());

    _float.link(vert, float_frag);
    _double.link(vert, double_frag);
}

static void process_defines(
    std::string &out,
    std::string_view src,
    const std::unordered_map<std::string_view, std::string_view> &def_map
) {
    constexpr std::string_view PREFIX = "#define ";

    for (auto l : std::ranges::views::split(src, '\n')) {
        const std::string_view line{ l };
        if (!line.starts_with(PREFIX)) {
            out.append(line);
            out.push_back('\n');
            continue;
        }

        const auto name_start = line.substr(PREFIX.size());
        const std::string_view name{ name_start.begin(),
                                     name_start.find(' ') };

        if (!def_map.contains(name)) {
            out.append(line);
            out.push_back('\n');
            continue;
        }

        out.append(PREFIX);
        out.append(name);
        out.push_back(' ');
        out.append(def_map.at(name));
        out.push_back('\n');
    }
}

} // namespace fio
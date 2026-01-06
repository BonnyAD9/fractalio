#pragma once

#include <vector>

#include "gl.hpp"
#include "shader.hpp"

namespace fio::gl {

class Program {
public:
    using Location = GLint;

    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    Program(Program &&other) noexcept {
        _id = other._id;
        other._id = 0;
    }

    Program &operator=(Program &&other) noexcept {
        std::swap(other._id, _id);
        return *this;
    }

    Program() : _id(glCreateProgram()) { }

    void compile_link(
        std::string_view vert_src, std::string_view frag_src
    ) const {
        Shader vert(GL_VERTEX_SHADER);
        Shader frag(GL_FRAGMENT_SHADER);
        vert.compile(vert_src);
        frag.compile(frag_src);
        link(vert, frag);
    }

    void attach(Shader &sh) const { glAttachShader(_id, sh.get()); }

    void link(Shader &vert, Shader &frag) const {
        attach(vert);
        attach(frag);
        link();
    }

    void link() const {
        glLinkProgram(_id);

        GLint success;
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        if (success == GL_FALSE) {
            GLint len;
            glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &len);
            std::vector<char> msg(len + 1, 0);
            glGetProgramInfoLog(_id, GLsizei(msg.size()), nullptr, msg.data());
            throw std::runtime_error(
                std::format("Failed to link shader: {}", msg.data())
            );
        }
    }

    GLint uniform_location(const char *name) const {
        return glGetUniformLocation(_id, name);
    }

    template<typename T> void uniform(GLint location, T value) {
        gl::uniform(location, value);
    }

    void use() const { glUseProgram(_id); }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static,modernize-use-nodiscard)
    constexpr bool use_double(bool) const { return false; }
    [[nodiscard]]
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    constexpr bool use_double() const {
        return false;
    }

    [[nodiscard]]
    GLuint get() const {
        return _id;
    }

    ~Program() {
        if (_id != 0) {
            glDeleteProgram(_id);
            _id = 0;
        }
    }

private:
    GLuint _id = 0;
};

} // namespace fio::gl

#pragma once

#include <print>
#include <vector>
#include "gl.hpp"
#include "shader.hpp"

namespace fio::gl {

class ShaderProgram {
public:
    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;

    ShaderProgram(ShaderProgram &&) = default;
    ShaderProgram &operator=(ShaderProgram &&) = default;

    ShaderProgram() : _id(glCreateProgram()) { }

    void construct(Shader &vert, Shader &frag) const {
        glAttachShader(_id, vert.get());
        glAttachShader(_id, frag.get());
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

    void use() const { glUseProgram(_id); }

    ~ShaderProgram() {
        if (_id != 0) {
            glDeleteProgram(_id);
            _id = 0;
        }
    }

private:
    GLuint _id;
};

} // namespace fio::gl
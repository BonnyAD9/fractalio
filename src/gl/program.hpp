#pragma once
#include <vector>

#include "gl.hpp"
#include "shader.hpp"

namespace fio::gl {

class Program {
public:
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

    void link(Shader &vert, Shader &frag) const {
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

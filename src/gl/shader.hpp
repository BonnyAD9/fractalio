#pragma once
#include <format>
#include <stdexcept>
#include <vector>

#include "gl.hpp"

namespace fio::gl {

class Shader {
public:
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    Shader(GLenum type) : _id(glCreateShader(type)) { }

    void compile(const char *source) const {
        glShaderSource(_id, 1, &source, nullptr);
        glCompileShader(_id);

        GLint success;
        glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            GLint len;
            glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &len);
            std::vector<char> msg(len + 1, 0);
            glGetShaderInfoLog(_id, GLsizei(msg.size()), nullptr, msg.data());
            throw std::runtime_error(
                std::format("Failed to link shader: {}", msg.data())
            );
        }
    }

    [[nodiscard]]
    GLuint get() const {
        return _id;
    }

    ~Shader() {
        if (_id != 0) {
            glDeleteShader(_id);
            _id = 0;
        }
    }

private:
    GLuint _id = 0;
};

} // namespace fio::gl
#pragma once

#include <array>
#include <format>
#include <iostream>
#include <print>
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
        if (!success) {
            std::array<char, 512> msg;
            glGetShaderInfoLog(_id, msg.size(), nullptr, msg.data());
            throw std::runtime_error(
                std::format("Failed to compile shader: {}", msg.data())
            );
        }
        GLint len;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &len);
        if (len != 0) {
            std::vector<char> msg(len + 1, 0);
            glGetShaderInfoLog(_id, GLsizei(msg.size()), nullptr, msg.data());
            std::println(std::cerr, "shader warning: {}", msg.data());
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
    GLuint _id;
};

} // namespace fio::gl
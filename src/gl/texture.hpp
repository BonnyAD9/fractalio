#pragma once

#include "gl.hpp"

namespace fio::gl {

class Texture {
public:
    Texture(Texture &) = delete;
    Texture(Texture &&other) noexcept : _id(other._id) { other._id = 0; }
    Texture &operator=(Texture &) = delete;
    Texture &operator=(Texture &&other) noexcept {
        std::swap(_id, other._id);
        return *this;
    }

    Texture() { glGenTextures(1, &_id); }

    void bind(GLenum type) const { glBindTexture(type, _id); }

    operator bool() const { return _id; }

    GLuint operator*() const { return _id; }

    ~Texture() {
        if (_id != 0) {
            glDeleteTextures(1, &_id);
            _id = 0;
        }
    }

private:
    GLuint _id;
};

} // namespace fio::gl
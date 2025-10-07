#pragma once

#include "gl.hpp"

namespace fio::gl {

class Texture {
public:
    Texture() { glGenTextures(1, &_id); }

    void bind(GLenum type) const { glBindTexture(type, _id); }

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
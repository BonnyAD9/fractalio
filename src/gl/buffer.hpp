#pragma once
#include <stdexcept>

#include "gl.hpp"

namespace fio::gl {

class Buffer {
public:
    Buffer(const Buffer &) = delete;
    Buffer &operator=(Buffer &) = delete;

    Buffer() = default;

    void init() {
        if (_id != 0) {
            throw std::runtime_error("Buffer initialized multiple times.");
        }
        glGenBuffers(1, &_id);
    }

    void bind(GLenum type) const { glBindBuffer(type, _id); }

    [[nodiscard]]
    GLuint get() const {
        return _id;
    }

    ~Buffer() {
        if (_id != 0) {
            glDeleteBuffers(1, &_id);
            _id = 0;
        }
    }

private:
    GLuint _id = 0;
};

} // namespace fio::gl
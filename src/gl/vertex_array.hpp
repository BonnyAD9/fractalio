#pragma once
#include <stdexcept>

#include "gl.hpp"

namespace fio::gl {

class VertexArray {
public:
    VertexArray(const VertexArray &) = delete;
    VertexArray &operator=(VertexArray &) = delete;

    VertexArray() { glGenVertexArrays(1, &_id); }

    [[nodiscard]]
    GLuint get() const {
        return _id;
    }

    void bind() const { glBindVertexArray(_id); }

    ~VertexArray() {
        if (_id != 0) {
            glDeleteVertexArrays(1, &_id);
            _id = 0;
        }
    }

private:
    GLuint _id = 0;
};

} // namespace fio::gl
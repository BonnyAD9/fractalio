#pragma once

#include <glad/gl.h>
#include <span>

#include <glm/glm.hpp>

// make sure that the includes are not reordered
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if true
#include <GLFW/glfw3.h>
#endif

namespace fio::gl {

static inline void clear_color(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

static inline void buffer_data(
    GLenum type,
    std::span<const char> data, GLenum usage = GL_STATIC_DRAW
) {
    glBufferData(type, GLsizeiptr(data.size()), data.data(), usage);
}

static inline void buffer_data(
    GLenum type,
    std::span<const float> data, GLenum usage = GL_STATIC_DRAW
) {
    buffer_data(
        type,
        { reinterpret_cast<const char *>(data.data()),
            data.size() * sizeof(*data.data()) },
        usage
    );
}

static inline void buffer_data(
    GLenum type,
    std::span<const glm::vec3> data, GLenum usage = GL_STATIC_DRAW
) {
    buffer_data(
        type,
        { reinterpret_cast<const char *>(data.data()),
            data.size() * sizeof(*data.data()) },
        usage
    );
}

static inline void vertex_attrib_pointer(
    GLuint location,
    GLint st_size,
    GLenum type,
    bool normalize,
    GLsizei stride,
    std::ptrdiff_t offset
) {
    glVertexAttribPointer(
        location,
        st_size,
        type,
        normalize ? GL_TRUE : GL_FALSE,
        stride,
        // NOLINTNEXTLINE(performance-no-int-to-ptr)
        reinterpret_cast<void *>(offset)
    );
}

} // namespace fio::gl
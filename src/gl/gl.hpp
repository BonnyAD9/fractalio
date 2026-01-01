#pragma once

#include <glad/gl.h>

// make sure that the includes are not reordered
// NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if)
#if true
#include <GLFW/glfw3.h>
#endif

#include <cstddef>
#include <span>

#include <glm/glm.hpp>

namespace fio::gl {

static inline void clear_color(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

static inline void buffer_data(
    GLenum type, std::span<const char> data, GLenum usage = GL_STATIC_DRAW
) {
    glBufferData(type, GLsizeiptr(data.size()), data.data(), usage);
}

static inline void buffer_data(
    GLenum type, std::span<const float> data, GLenum usage = GL_STATIC_DRAW
) {
    buffer_data(
        type,
        { reinterpret_cast<const char *>(data.data()),
          data.size() * sizeof(*data.data()) },
        usage
    );
}

static inline void buffer_data(
    GLenum type, std::span<const glm::vec3> data, GLenum usage = GL_STATIC_DRAW
) {
    buffer_data(
        type,
        { reinterpret_cast<const char *>(data.data()),
          data.size() * sizeof(*data.data()) },
        usage
    );
}

static inline void buffer_data(
    GLenum type, std::span<const GLuint> data, GLenum usage = GL_STATIC_DRAW
) {
    buffer_data(
        type,
        { reinterpret_cast<const char *>(data.data()),
          data.size() * sizeof(*data.data()) },
        usage
    );
}

static inline void draw_buffers(std::span<const GLenum> bufs) {
    glDrawBuffers(GLsizei(bufs.size()), bufs.data());
}

static inline void draw_buffer(GLenum buf) {
    glDrawBuffer(buf);
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

static inline void draw_elements(
    GLenum el_type, GLsizei count, GLenum type, std::ptrdiff_t offset
) {
    glDrawElements(
        el_type,
        count,
        type,
        // NOLINTNEXTLINE(performance-no-int-to-ptr)
        reinterpret_cast<void *>(offset)
    );
}

static inline void draw_arrays(GLenum mode, GLint first, GLsizei count) {
    glDrawArrays(mode, first, count);
}

static inline void tex_image_1d(
    GLuint t_type, GLint ifmt, GLenum fmt, GLsizei width, const char *data
) {
    glTexImage1D(t_type, 0, ifmt, width, 0, fmt, GL_UNSIGNED_BYTE, data);
}

static inline void tex_image_1d(std::span<const glm::u8vec3> data) {
    tex_image_1d(
        GL_TEXTURE_1D,
        GL_RGB,
        GL_RGB,
        GLsizei(data.size()),
        reinterpret_cast<const char *>(data.data())
    );
}

static inline void tex_image_2d(
    const std::uint8_t *data, GLsizei w, GLsizei h, GLenum typ
) {
    glTexImage2D(
        GL_TEXTURE_2D, 0, GLint(typ), w, h, 0, typ, GL_UNSIGNED_BYTE, data
    );
}

static inline void tex_image_2d(
    const std::uint8_t *data,
    GLsizei w,
    GLsizei h,
    GLint ct,
    GLenum comp,
    GLenum typ
) {
    glTexImage2D(GL_TEXTURE_2D, 0, ct, w, h, 0, comp, typ, data);
}

static inline void uniform(GLint location, std::span<glm::vec2> value) {
    glUniform2fv(
        location,
        GLsizei(value.size()),
        reinterpret_cast<const float *>(value.data())
    );
}

static inline void uniform(GLint location, glm::dvec2 value) {
    glUniform2d(location, value.x, value.y);
}

static inline void uniform(GLint location, glm::vec2 value) {
    glUniform2f(location, value.x, value.y);
}

static inline void uniform(GLint location, glm::vec3 value) {
    glUniform3f(location, value.x, value.y, value.z);
}

static inline void uniform(GLint location, glm::vec4 value) {
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

static inline void uniform(GLint location, double value) {
    glUniform1d(location, value);
}

static inline void uniform(GLint location, float value) {
    glUniform1f(location, value);
}

static inline void uniform(GLint location, GLuint value) {
    glUniform1ui(location, value);
}

static inline void uniform(GLint location, glm::mat4 value) {
    glUniformMatrix4fv(location, 1, false, &value[0][0]);
}

} // namespace fio::gl
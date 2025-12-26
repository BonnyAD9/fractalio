#pragma once

#include "gl.hpp"
#include "glad/gl.h"
#include "texture.hpp"

namespace fio::gl {

class Framebuffer {
public:
    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(Framebuffer &) = delete;

    Framebuffer() { glGenFramebuffers(1, &_id); }

    void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, _id); }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void texture(
        gl::Texture &texture,
        GLenum idx = GL_COLOR_ATTACHMENT0,
        GLint mip_level = 0
    ) {
        glFramebufferTexture(GL_FRAMEBUFFER, idx, *texture, mip_level);
    }

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    void texture_2d(
        gl::Texture &texture,
        GLenum idx = GL_COLOR_ATTACHMENT0,
        GLint mip_level = 0
    ) {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, idx, GL_TEXTURE_2D, *texture, mip_level
        );
    }

private:
    GLuint _id;
};

} // namespace fio::gl
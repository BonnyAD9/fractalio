#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <freetype/freetype.h>
#include <freetype/fttypes.h>

#include "../gl/texture.hpp"

#include <glm/glm.hpp>

namespace fio::ft {

class Font {
public:
    Font(const Font &) = delete;
    Font(Font &&) = delete;
    Font &operator=(const Font &) = delete;
    Font &operator=(Font &&) = delete;

    Font(const char *name, FT_UInt size);

    struct Glyph {
        glm::uvec2 tpos;
        glm::uvec2 tsize;
        glm::ivec2 bearing;
        unsigned advance;
    };

    std::uint8_t *data() { return _texture.data(); }

    [[nodiscard]]
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    constexpr std::size_t width() const {
        return 512;
    }

    [[nodiscard]]
    constexpr std::size_t height() const {
        return _texture.size() / width();
    }

    constexpr glm::vec2 size() const { return { width(), height() }; }

    Glyph &get(char c) {
        auto res = _glyphs.find(c);
        if (res == _glyphs.end()) {
            return _unknown;
        }
        return res->second;
    }

    void use() { _gl_texture.bind(GL_TEXTURE_2D); }

private:
    std::unordered_map<char, Glyph> _glyphs;
    Glyph _unknown;

    std::vector<std::uint8_t> _texture;
    std::size_t _line_y = 0;
    std::size_t _line_x = 0;

    gl::Texture _gl_texture;

    Glyph make_glyph(FT_GlyphSlot glyph);
    glm::uvec2 add_render(std::uint8_t *data, glm::uvec2 size);
};

} // namespace fio
#include "font.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include <fontconfig/fontconfig.h>

#include "fc/config.hpp"
#include "fc/font_set.hpp"
#include "fc/object_set.hpp"
#include "fc/pattern.hpp"
#include "ft/face.hpp"
#include "ft/library.hpp"

namespace fio {

static std::string find_font(const char *name);

Font::Font(const char *name, FT_UInt size) {
    auto path = find_font(name);

    ft::Library lib;
    auto face = lib.new_face(path.c_str());
    face.set_pixel_sizes(size);

    face.load_char(0xfffd);
    _unknown = make_glyph(face->glyph);

    for (char c = ' '; c < 0x7f; ++c) {
        face.load_char(c);
        _glyphs[c] = make_glyph(face->glyph);
    }

    _gl_texture.bind(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl::tex_image_2d(data(), GLsizei(width()), GLsizei(height()), GL_RED);
}

Font::Glyph Font::make_glyph(FT_GlyphSlot glyph) {
    const glm::uvec2 tsize{ glyph->bitmap.width, glyph->bitmap.rows };
    const glm::uvec2 tpos = add_render(glyph->bitmap.buffer, tsize);
    return {
        .tpos = tpos,
        .tsize = tsize,
        .bearing{ glyph->bitmap_left, glyph->bitmap_top },
        .advance = unsigned(glyph->advance.x),
    };
}

glm::uvec2 Font::add_render(std::uint8_t *data, glm::uvec2 size) {
    const std::size_t w = size.x;
    const std::size_t h = size.y;

    if (w > width()) {
        throw std::runtime_error("Gliph is too large to fit in the bitmap.");
    }

    if (_line_x + w > width()) {
        _line_x = 0;
        _line_y = height();
    }

    auto new_h = _line_y + h;

    if (new_h > height()) {
        _texture.resize(new_h * width());
    }

    std::size_t pos = _line_y * width() + _line_x;
    const std::size_t pos_end = new_h * width() + _line_x;
    std::size_t gpos = 0;
    for (; pos < pos_end; pos += width(), gpos += w) {
        std::copy_n(data + gpos, w, _texture.data() + pos);
    }

    const glm::uvec2 res{ _line_x, _line_y };
    _line_x += w;
    return res;
}

static std::string find_font(const char *name) {
    fc::Config conf;
    fc::Pattern pat(name);
    conf.subtitude(pat);
    pat.default_substitude();
    fc::FontSet fs;
    fc::ObjectSet os{ FC_FAMILY, FC_STYLE, FC_FILE };

    FcResult result;
    auto patterns = conf.font_sort(pat, false, nullptr, result);
    if (patterns->nfont == 0) {
        throw std::runtime_error("There are no fonts on this system.");
    }

    auto font_pattern = conf.font_render_prepare(pat, *patterns->fonts[0]);
    fs.add(std::move(font_pattern));

    FcValue v;
    fc::Pattern filter = FcPatternFilter(fs->fonts[0], os.get());

    FcPatternGet(filter.get(), FC_FILE, 0, &v);
    return { reinterpret_cast<const char *>(v.u.f) };
}

} // namespace fio
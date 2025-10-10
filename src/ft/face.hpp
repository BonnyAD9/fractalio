#pragma once

#include <memory>
#include <type_traits>

#include <freetype/freetype.h>

#include "check_error.hpp"

#include <glm/glm.hpp>

namespace fio::ft {

namespace del {
struct FTFace {
    void operator()(FT_Face f) { FT_Done_Face(f); }
};
} // namespace del

class Face {
public:
    Face(FT_Face face) : _face(face) { }

    FT_Face get() { return _face.get(); }

    FT_Face operator->() { return get(); }

    void set_pixel_sizes(FT_UInt w, FT_UInt h) {
        FT_Set_Pixel_Sizes(get(), w, h);
    }

    void set_pixel_sizes(FT_UInt h) { set_pixel_sizes(0, h); }

    void load_char(char c, FT_Int32 flags = FT_LOAD_RENDER) {
        check_error(FT_Load_Char(get(), c, flags));
    }

private:
    std::unique_ptr<std::remove_pointer_t<FT_Face>, del::FTFace> _face;
};

} // namespace fio::ft
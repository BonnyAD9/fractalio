#pragma once

#include <memory>
#include <type_traits>

#include <freetype/freetype.h>

#include "check_error.hpp"
#include "face.hpp"

namespace fio::ft {

namespace del {
struct FTLibrary {
    void operator()(::FT_Library lib) { FT_Done_FreeType(lib); }
};
} // namespace del

class Library {
public:
    Library() {
        FT_Library lib;
        check_error(FT_Init_FreeType(&lib));
        _lib =
            std::unique_ptr<std::remove_pointer_t<FT_Library>, del::FTLibrary>(
                lib
            );
    }

    FT_Library get() { return _lib.get(); }

    Face new_face(const char *path, FT_Long index = 0) {
        FT_Face face;
        check_error(FT_New_Face(get(), path, index, &face));
        return { face };
    }

private:
    std::unique_ptr<std::remove_pointer_t<FT_Library>, del::FTLibrary> _lib;
};

} // namespace fio::ft
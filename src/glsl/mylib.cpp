#include "mylib.hpp"
#include <optional>
#include <unordered_map>
#include "glad/gl.h"

namespace fio::glsl {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char MB_COLORING[]{
#embed "mylib/mb_coloring.glsl"
    , 0
};

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char PLOAT[]{
#embed "mylib/ploat.glsl"
    , 0
};
    
const std::unordered_map<std::string_view, std::string_view> &mylib_includes() {
    static std::optional<std::unordered_map<std::string_view, std::string_view>> mylib;
    
    if (!mylib) {
        mylib = {
            {"mb_coloring", MB_COLORING},
            {"ploat", PLOAT},
        };
    }
    
    return *mylib;
}
    
}
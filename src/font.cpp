#include "font.hpp"
#include <filesystem>

#include <fontconfig/fontconfig.h>
#include "fc/config.hpp"
#include "fc/font_set.hpp"
#include "fc/object_set.hpp"
#include "fc/pattern.hpp"

namespace fio {
    
static std::filesystem::path find_font();
    
Font::Font() {
    
}

static std::filesystem::path find_font() {
    fc::Config conf;
    fc::Pattern pat("monospace");
    conf.subtitude(pat);
    pat.default_substitude();
    fc::FontSet fs;
    fc::ObjectSet os{FC_FAMILY, FC_STYLE, FC_FILE};
    
    FcResult result;
    auto patterns = conf.font_sort(pat, false, nullptr, result);
}
    
}
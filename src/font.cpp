#include "font.hpp"

#include <filesystem>
#include <print>
#include <stdexcept>

#include <fontconfig/fontconfig.h>

#include "fc/config.hpp"
#include "fc/font_set.hpp"
#include "fc/object_set.hpp"
#include "fc/pattern.hpp"

namespace fio {

static std::string find_font();

Font::Font() {
    std::println("{}", find_font());
}

static std::string find_font() {
    fc::Config conf;
    fc::Pattern pat("monospace");
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
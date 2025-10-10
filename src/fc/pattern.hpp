#pragma once

#include <memory>
#include <stdexcept>

#include <fontconfig/fontconfig.h>

#include "../make_deleter.hpp"

namespace fio::fc {

MAKE_DELETER(FcPattern, FcPatternDestroy)

class Pattern {
public:
    Pattern(const char *name) :
        _pat(FcNameParse(reinterpret_cast<const FcChar8 *>(name))) {
        if (!_pat) {
            throw std::runtime_error("Failed to parse font pattern.");
        }
    }
    Pattern(FcPattern *pat) : _pat(pat) { }

    FcPattern *get() { return _pat.get(); }

    void default_substitude() { FcDefaultSubstitute(get()); }

    FcPattern *release() { return _pat.release(); }

private:
    std::unique_ptr<FcPattern, del::FcPattern> _pat;
};

} // namespace fio::fc
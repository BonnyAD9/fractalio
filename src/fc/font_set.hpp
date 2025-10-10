#pragma once

#include <memory>

#include <fontconfig/fontconfig.h>

#include "../make_deleter.hpp"
#include "pattern.hpp"

namespace fio::fc {

MAKE_DELETER(FcFontSet, FcFontSetDestroy)

class FontSet {
public:
    FontSet() : _set(FcFontSetCreate()) { }
    FontSet(FcFontSet *set) : _set(set) { }

    FcFontSet *get() { return _set.get(); }

    FcFontSet *operator->() { return get(); }

    void add(Pattern pat) { FcFontSetAdd(get(), pat.release()); }

private:
    std::unique_ptr<FcFontSet, del::FcFontSet> _set;
};

} // namespace fio::fc
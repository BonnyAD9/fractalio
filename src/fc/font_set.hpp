#pragma once

#include <memory>
#include <fontconfig/fontconfig.h>
#include "../make_deleter.hpp"

namespace fio::fc {
    
MAKE_DELETER(FcFontSet, FcFontSetDestroy)
    
class FontSet {
public:
    FontSet() : _set(FcFontSetCreate()) {}
    FontSet(FcFontSet *set) : _set(set) {}

    FcFontSet *get() {
        return _set.get();
    }
    
private:
    std::unique_ptr<FcFontSet, del::FcFontSet> _set;
};
    
}
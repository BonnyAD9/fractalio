#pragma once

#include <memory>
#include <stdexcept>
#include <fontconfig/fontconfig.h>
#include "../make_deleter.hpp"
#include "font_set.hpp"
#include "pattern.hpp"

namespace fio::fc {
    
MAKE_DELETER(FcConfig, FcConfigDestroy)

class Config {
public:
    Config() : _conf(FcInitLoadConfigAndFonts()) {
        
    }
    
    FcConfig *get() {
        return _conf.get();
    }
    
    void subtitude(Pattern &pat, FcMatchKind kind = FcMatchPattern) {
        FcConfigSubstitute(get(), pat.get(), kind);
    }
    
    FontSet font_sort(Pattern &pat, bool trim, FcCharSet **csp, FcResult &result) {
        auto res = FcFontSort(get(), pat.get(), trim ? FcTrue : FcFalse, csp, &result);
        if (!res) {
            throw std::runtime_error("Failed to find any fonts.");
        }
        return { res };
    }

private:
    std::unique_ptr<FcConfig, del::FcConfig> _conf;
};
    
}
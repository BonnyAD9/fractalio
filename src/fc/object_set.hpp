#pragma once

#include <memory>

#include <fontconfig/fontconfig.h>

#include "../make_deleter.hpp"

namespace fio::fc {

MAKE_DELETER(FcObjectSet, FcObjectSetDestroy)

class ObjectSet {
public:
    template<typename... Pars>
    ObjectSet(Pars... pars) : _set(FcObjectSetBuild(pars..., nullptr)) { }

    FcObjectSet *get() { return _set.get(); }

private:
    std::unique_ptr<FcObjectSet, del::FcObjectSet> _set;
};

} // namespace fio::fc
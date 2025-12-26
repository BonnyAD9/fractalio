#pragma once

#include <string>
#include <unordered_map>

#include "mylib.hpp"

namespace fio::glsl {

void preprocess_defines(
    std::string &out,
    std::string_view src,
    const std::unordered_map<std::string_view, std::string_view> &def_map
);

void preprocess_includes(
    std::string &out,
    std::string_view src,
    const std::unordered_map<std::string_view, std::string_view> &inc_map
);

static inline std::string preprocess_mylib(std::string_view src) {
    std::string res;
    preprocess_includes(res, src, mylib_includes());
    return res;
}

} // namespace fio::glsl
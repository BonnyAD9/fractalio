#pragma once

#include <unordered_map>
#include <string_view>

namespace fio::glsl {
    
const std::unordered_map<std::string_view, std::string_view> &mylib_includes();
    
}
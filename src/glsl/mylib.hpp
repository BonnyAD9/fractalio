#pragma once

#include <string_view>
#include <unordered_map>

namespace fio::glsl {

const std::unordered_map<std::string_view, std::string_view> &mylib_includes();

}
#pragma once

#include <string>
#include <unordered_map>
namespace fio::gl {

void process_defines(
    std::string &out,
    std::string_view src,
    const std::unordered_map<std::string_view, std::string_view> &def_map
);

}
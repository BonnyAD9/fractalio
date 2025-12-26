#include "preprocess.hpp"

#include <algorithm>
#include <format>
#include <print>
#include <ranges>
#include <stdexcept>

namespace fio::glsl {

void preprocess_defines(
    std::string &out,
    std::string_view src,
    const std::unordered_map<std::string_view, std::string_view> &def_map
) {
    constexpr std::string_view PREFIX = "#define ";

    for (auto l : std::ranges::views::split(src, '\n')) {
        const std::string_view line{ l };
        if (!line.starts_with(PREFIX)) {
            out.append(line);
            out.push_back('\n');
            continue;
        }

        const auto name_start = line.substr(PREFIX.size());
        const std::string_view name{ name_start.begin(),
                                     std::ranges::find(name_start, ' ') };

        if (!def_map.contains(name)) {
            out.append(line);
            out.push_back('\n');
            continue;
        }

        out.append(PREFIX);
        out.append(name);
        out.push_back(' ');
        out.append(def_map.at(name));
        out.push_back('\n');
    }
}

void preprocess_includes(std::string &out, std::string_view src, const std::unordered_map<std::string_view, std::string_view> &inc_map) {
    constexpr std::string_view PREFIX = "#include ";
    
    for (auto l : std::ranges::views::split(src, '\n')) {
        const std::string_view line{ l };
        if (!line.starts_with(PREFIX)) {
            out.append(line);
            out.push_back('\n');
            continue;
        }
        
        auto file = line.substr(PREFIX.size() + 1);
        file = file.substr(0, file.length() - 1);
        std::string_view contents;
        try {
            contents = inc_map.at(file);
        } catch (...) {
            throw std::runtime_error(std::format("Invalid include `{}` in line `{}`", file, line));
        }
        preprocess_includes(out, contents, inc_map);
        out.push_back('\n');
    }
}

} // namespace fio::gl
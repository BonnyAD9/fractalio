#include "process_defines.hpp"

#include <ranges>

namespace fio::gl {

void process_defines(
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
                                     name_start.find(' ') };

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

} // namespace fio::gl
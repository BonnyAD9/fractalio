#pragma once

#include <span>
#include <string>
#include <vector>
namespace fio::cli {

struct Args {
    Args(std::span<char *> arguments);

    bool should_exit = false;
    std::vector<std::string> commands;
};

} // namespace fio::cli
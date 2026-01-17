#include "args.hpp"

#include <print>
#include <ranges>
#include <span>
#include <string_view>

#include "../fractals/help.hpp"
#include "../pareg/pareg.hpp"

namespace fio::cli {

static void help();

Args::Args(std::span<char *> arguments) {
    auto marguments = std::views::transform(arguments, [](auto a) {
        return std::string_view(a);
    });
    auto args = pareg::create(marguments.begin(), marguments.end());

    while (!args.empty()) {
        auto arg = args.next_arg<std::string_view>();
        if (arg == "-h" || arg == "--help") {
            help();
            should_exit = true;
        } else if (arg == "--cmd-help") {
            std::print("{}", fractals::help_text());
            should_exit = true;
        } else if (arg == "-l" || arg == "--load") {
            commands.push_back(
                std::format(":load {}", args.next_arg<std::string_view>())
            );
        } else if (arg == "-c" || arg == "--cmd" || arg == "--command") {
            commands.push_back(args.next_arg<std::string>());
        } else {
            commands.emplace_back(arg);
        }
    }
}

static void help() {
    std::print(R".(Welcome in help for fractalio by BonnyAD9.
Version: 1.0.0

Usage:
  fractalio [flags]
    Starts fractalio in the configuration given by flags.

Flags:
  -h  --help
    Show this help.

  --cmd-help
    Print command help.

  -l  --load <file>
    Load configuration from file. `-` is stdin.

  -c  --cmd  --command <command>
    Run the given command after startup.
).");
}

} // namespace fio::cli
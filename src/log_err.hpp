#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include <print>

namespace fio {

static inline bool log_err(const std::function<void(void)> &f) noexcept {
    try {
        try {
            f();
            return true;
        } catch (std::exception &ex) {
            std::println(std::cerr, "error: {}", ex.what());
        } catch (...) {
            std::println("An unknown error occured.");
        }
        // NOLINTNEXTLINE(bugprone-empty-catch)
    } catch (...) {
        // Nothing to do here
    }
    return false;
}

} // namespace fio
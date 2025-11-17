#pragma once

#include <charconv>
#include <concepts>
#include <format>
#include <iostream>
#include <print>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace mdt::pareg {

template<typename T> struct Parse {
    T parse(std::string_view) {
        static_assert(false, "Parse not implemented.");
    }
};

template<typename T, typename... Args> T from_arg(std::string_view, Args...) {
    static_assert(false, "From arg not implemented.");
}

template<typename T, typename... Args>
concept FromParse = requires(std::string_view arg, Args... args) {
    { Parse<T>(std::forward<Args>(args)...).parse(arg) } -> std::same_as<T>;
};

template<typename T, typename... Args>
    requires FromParse<T, Args...>
T from_arg(std::string_view arg, Args... args) {
    return Parse<T>(std::forward<Args>(args)...).parse(arg);
}

template<std::constructible_from<std::string_view> T>
T from_arg(std::string_view arg) {
    return T(arg);
}

template<typename T>
concept FromChars = requires(std::string_view arg, T &v) {
    T();
    {
        std::from_chars(arg.begin(), arg.end(), v)
    } -> std::same_as<std::from_chars_result>;
};

template<FromChars T> T from_arg(std::string_view arg) {
    T value;
    auto res = std::from_chars(arg.begin(), arg.end(), value);
    if (res.ptr != arg.end()) {
        throw std::runtime_error("Failed to parse with from_chars.");
    }
    return value;
}

template<typename T, typename... Args>
concept Parsable = requires(std::string_view arg, Args... args) {
    {
        from_arg<T, Args...>(arg, std::forward<Args>(args)...)
    } -> std::same_as<T>;
};

template<>
inline bool from_arg<bool, std::string_view, std::string_view>(
    std::string_view arg, std::string_view t, std::string_view f
) {
    if (arg == t) {
        return true;
    }
    if (arg == f) {
        return false;
    }
    throw std::runtime_error(
        std::format(
            "Invalid boolean value `{}`. Expected `{}` or `{}`", arg, t, f
        )
    );
}

} // namespace mdt::pareg
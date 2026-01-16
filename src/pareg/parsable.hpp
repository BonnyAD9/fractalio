#pragma once

#include <charconv>
#include <concepts>
#include <format>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

namespace fio::pareg {

template<typename T, typename... TArgs> struct Parse {
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

static inline char *float_from_chars(const char *s, float &f) {
    char *e;
    f = std::strtof(s, &e);
    return e;
}

static inline char *float_from_chars(const char *s, double &f) {
    char *e;
    f = std::strtod(s, &e);
    return e;
}

static inline char *float_from_chars(const char *s, long double &f) {
    char *e;
    f = std::strtold(s, &e);
    return e;
}

template<std::floating_point T> T from_arg(std::string_view arg) {
    std::string sarg{ arg };
    if (sarg.starts_with("0x-") || sarg.starts_with("0X-")) {
        sarg[2] = sarg[1];
        sarg[0] = '-';
        sarg[1] = '0';
    }
    T value;
    auto res = float_from_chars(sarg.c_str(), value);
    if (*res) {
        throw std::runtime_error("Failed to parse float with from_chars.");
    }
    return value;
}

template<typename T>
concept FromChars = requires(std::string_view arg, T &v) {
    T();
    {
        std::from_chars(arg.begin(), arg.end(), v)
    } -> std::same_as<std::from_chars_result>;
} && !std::floating_point<T>;

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

template<typename T> T to_digit_base(char c, int base) {
    T res = 0;
    if (c >= '0' && c <= '9') {
        res = c - '0';
    } else if (c >= 'a' && c <= 'z') {
        res = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'Z') {
        res = c - 'A' + 10;
    } else {
        throw std::runtime_error(std::format("Invalid digit `{}`.", c));
    }
    if (res >= base) {
        throw std::runtime_error(std::format("Invalid digit `{}`.", c));
    }
    return res;
}

template<typename T> T parse_int_base(std::string_view arg, int base) {
    T res = 0;
    for (auto d : arg) {
        res = res * base + to_digit_base<T>(d, base);
    }
    return res;
}

template<> inline glm::u8vec3 from_arg<glm::u8vec3>(std::string_view arg) {
    if (!arg.starts_with('#')) {
        throw std::runtime_error(
            std::format("Expected # to start color but got `{}`", arg)
        );
    }
    arg = arg.substr(1);
    if (arg.size() != 6) {
        throw std::runtime_error(
            "Expected color to have 3 two digit components."
        );
    }
    return {
        parse_int_base<std::uint8_t>(arg.substr(0, 2), 16),
        parse_int_base<std::uint8_t>(arg.substr(2, 2), 16),
        parse_int_base<std::uint8_t>(arg.substr(4, 2), 16),
    };
}

template<Parsable A, Parsable B> struct Parse<std::pair<A, B>, A, B> {
    Parse() : _sep(":") { }
    Parse(std::string_view sep) : _sep(sep) { }

    std::pair<A, B> parse(std::string_view arg) {
        auto pos = arg.find(_sep);
        if (pos == std::string_view::npos) {
            throw std::runtime_error("Missing pair separator");
        }
        return {
            from_arg<A>(arg.substr(0, pos)),
            from_arg<B>(arg.substr(pos + _sep.size())),
        };
    }

private:
    std::string_view _sep;
};

template<typename T> struct Parse<std::vector<T>, T> {
    Parse() : _sep(",") { }
    Parse(std::string_view sep) : _sep(sep) { }

    std::vector<T> parse(std::string_view arg) {
        std::vector<T> res;
        for (auto v : std::ranges::split_view(arg, _sep)) {
            std::string_view s{ v };
            res.push_back(from_arg<T>(s));
        }
        return res;
    }

private:
    std::string_view _sep;
};

template<>
inline std::vector<std::pair<float, glm::u8vec3>>
from_arg<std::vector<std::pair<float, glm::u8vec3>>>(std::string_view arg) {
    std::vector<std::pair<float, glm::u8vec3>> res;
    for (auto v : std::ranges::views::split(arg, ',')) {
        const std::string_view s{ v };
        res.push_back(
            Parse<std::pair<float, glm::u8vec3>, float, glm::u8vec3>().parse(s)
        );
    }
    return res;
}

} // namespace fio::pareg
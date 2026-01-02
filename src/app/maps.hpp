#pragma once

#include <cmath>
#include <functional>
#include <optional>

namespace fio::app::maps {

template<typename T> static inline std::function<T(T)> value(T v) {
    return [=](T) { return v; };
}

template<typename T> static inline T dble(T v) {
    return v == 0 ? T(1) : T(v * 2);
}

template<typename T> static inline T half(T v) {
    return v / 2;
}

template<typename T> static inline std::function<T(T)> add(T v) {
    return [=](T p) { return p + v; };
}

template<typename T> static inline std::function<T(T)> mul(T v) {
    return [=](T p) { return std::max(p * v, T(1)); };
}

template<typename T> static inline T reset(T) {
    return NAN;
}

static inline bool negate(bool v) {
    return !v;
}

template<typename T>
static inline std::function<T(T)> inverse(const std::function<T(T)> &f) {
    return [&](T v) { return 1 / f(v == 0 ? 0 : 1 / v); };
}

template<typename T>
static inline std::function<T(T)> modified(
    char modifier, std::optional<T> num, std::function<T(T)> def
) {
    switch (modifier) {
    case '+':
        return add(num.value_or(1.));
    case '-':
        return add(-num.value_or(1.));
    case '*':
        return mul(num.value_or(2.));
    case '/':
        return mul(1 / num.value_or(2.));
    case '=':
        if (num) {
            return value(*num);
        }
        return reset<T>;
    default:
        if (num) {
            return value(*num);
        }
        return def;
    }
}

} // namespace fio::app::maps
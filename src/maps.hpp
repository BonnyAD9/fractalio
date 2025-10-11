#pragma once

#include <cmath>
#include <functional>
#include <optional>

namespace fio::maps {

static inline std::function<float(float)> value(float v) {
    return [=](float) { return v; };
}

static inline float dble(float v) {
    return std::max(v * 2.F, 1.F);
}

static inline float half(float v) {
    return v / 2;
}

static inline std::function<float(float)> add(float v) {
    return [=](float p) { return p + v; };
}

static inline std::function<float(float)> mul(float v) {
    return [=](float p) { return std::max(p * v, 1.F); };
}

static inline float reset(float) {
    return NAN;
}

static inline bool negate(bool v) {
    return !v;
}

static inline std::function<float(float)> modified(
    char modifier, std::optional<float> num, std::function<float(float)> def
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
        return reset;
    default:
        if (num) {
            return value(*num);
        }
        return def;
    }
}

} // namespace fio::maps
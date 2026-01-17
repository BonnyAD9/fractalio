#include "gradient.hpp"

#include <algorithm>
#include <array>
#include <cassert>

#include "pareg/parsable.hpp"

namespace fio::gradient {

void linear_gradient(
    glm::u8vec3 start, glm::u8vec3 end, std::span<glm::u8vec3> dst
) {
    glm::vec3 i = start;
    const glm::vec3 step = (glm::vec3(end) - i) / float(dst.size());
    for (; !dst.empty(); i += step) {
        dst[0] = i;
        dst = dst.subspan(1);
    }
}

void linear_gradient(
    std::span<std::pair<float, glm::u8vec3>> pts, std::span<glm::u8vec3> dst
) {
    assert(!pts.empty());
    if (pts.size() == 1) {
        std::ranges::fill(dst, pts[0].second);
        return;
    }

    auto start = std::size_t(pts[0].first * float(dst.size()));
    for (std::size_t i = 1; i < pts.size(); ++i) {
        auto end = std::size_t(pts[i].first * float(dst.size()));
        linear_gradient(
            pts[i - 1].second, pts[i].second, dst.subspan(start, end - start)
        );
        start = end;
    }

    auto to_first = pts[0].first * float(dst.size());
    auto lasti = pts.rbegin()->first * float(dst.size());
    auto from_last = float(dst.size()) - lasti;

    glm::u8vec3 boundary;
    if (from_last == 0) {
        boundary = pts.rbegin()->second;
    } else {
        auto cnt = from_last + to_first;
        boundary = glm::vec3(pts.rbegin()->second) * to_first / cnt +
                   glm::vec3(pts[0].second) * from_last / cnt;

        linear_gradient(
            pts.rbegin()->second, boundary, dst.subspan(std::size_t(lasti))
        );
    }

    if (to_first != 0) {
        linear_gradient(
            boundary, pts[0].second, dst.subspan(0, std::size_t(to_first))
        );
    }
}

std::vector<glm::u8vec3> from_string(std::string_view gradient) {
    std::vector<glm::u8vec3> grad(256);
    if (gradient == "ultra-fractal") {
        gradient::ultra_fractal(grad);
    } else if (gradient == "grayscale") {
        gradient::grayscale(grad);
    } else if (gradient == "burn") {
        gradient::burn(grad);
    } else if (gradient == "monokai") {
        gradient::monokai(grad);
    } else if (gradient == "rgb") {
        gradient::rgb(grad);
    } else if (gradient == "cmy") {
        gradient::cmy(grad);
    } else {
        auto sep = gradient.find('@');
        if (sep != std::string_view::npos) {
            auto siz = pareg::from_arg<std::size_t>({ gradient.begin(), sep });
            grad.resize(siz);
            gradient = gradient.substr(sep + 1);
        }
        auto pts = pareg::from_arg<std::vector<std::pair<float, glm::u8vec3>>>(
            gradient
        );
        gradient::linear_gradient(pts, grad);
    }
    return grad;
}

void ultra_fractal(std::span<glm::u8vec3> dst) {
    std::array<std::pair<float, glm::u8vec3>, 5> pts{ {
        { 0, { 0, 7, 100 } },
        { 0.16, { 32, 107, 203 } },
        { 0.42, { 237, 255, 255 } },
        { 0.6425, { 255, 170, 0 } },
        { 0.8575, { 0, 2, 0 } },
    } };

    linear_gradient(pts, dst);
}

void grayscale(std::span<glm::u8vec3> dst) {
    std::array<std::pair<float, glm::u8vec3>, 2> pts{ {
        { 0, { 0, 0, 0 } },
        { 0.5, { 255, 255, 255 } },
    } };

    linear_gradient(pts, dst);
}

void burn(std::span<glm::u8vec3> dst) {
    std::array<std::pair<float, glm::u8vec3>, 4> pts{ {
        { 0, { 0, 0, 0 } },
        { 0.25, { 255, 0, 0 } },
        { 0.5, { 255, 255, 0 } },
        { 0.75, { 255, 255, 255 } },
    } };

    linear_gradient(pts, dst);
}

void monokai(std::span<glm::u8vec3> dst) {
    std::array<std::pair<float, glm::u8vec3>, 6> pts{ {
        { 0, { 0xFC, 0x5D, 0x7C } },
        { 0.17, { 0xF3, 0x96, 0x60 } },
        { 0.33, { 0xE7, 0xC6, 0x64 } },
        { 0.5, { 0x9E, 0xD0, 0x72 } },
        { 0.67, { 0x76, 0xCC, 0xE0 } },
        { 0.83, { 0xB3, 0x9D, 0xF3 } },
    } };

    linear_gradient(pts, dst);
}

void rgb(std::span<glm::u8vec3> dst) {
    std::array<std::pair<float, glm::u8vec3>, 3> pts{ {
        { 0, { 255, 0, 0 } },
        { 0.33, { 0, 255, 0 } },
        { 0.67, { 0, 0, 255 } },
    } };

    linear_gradient(pts, dst);
}

void cmy(std::span<glm::u8vec3> dst) {
    std::array<std::pair<float, glm::u8vec3>, 3> pts{ {
        { 0, { 0, 255, 255 } },
        { 0.33, { 255, 0, 255 } },
        { 0.67, { 255, 255, 0 } },
    } };

    linear_gradient(pts, dst);
}

} // namespace fio::gradient
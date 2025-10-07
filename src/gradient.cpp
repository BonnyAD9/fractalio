#include "gradient.hpp"

#include <algorithm>
#include <array>
#include <cassert>

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

} // namespace fio::gradient
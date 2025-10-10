#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <freetype/fttypes.h>

#include <glm/glm.hpp>

namespace fio {

class Font {
public:
    Font(const char *name, FT_UInt size);

    struct Gliph {
        glm::uvec2 tpos;
        glm::uvec2 tsize;
        glm::uvec2 bearing;
        unsigned advance;
    };

    std::uint8_t *data() { return _texture.data(); }

    [[nodiscard]]
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    constexpr std::size_t width() const {
        return 512;
    }

    [[nodiscard]]
    constexpr std::size_t height() const {
        return _texture.size() / width();
    }

private:
    std::unordered_map<char, Gliph> _gliphs;

    std::vector<std::uint8_t> _texture;
    std::size_t _line_y = 0;
    std::size_t _line_x = 0;

    glm::uvec2 add_render(std::uint8_t *data, glm::uvec2 size);
};

} // namespace fio
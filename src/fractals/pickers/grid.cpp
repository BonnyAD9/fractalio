#include "grid.hpp"

namespace fio::fractals::pickers {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "grid.frag"
    , 0
};

Grid::Grid(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    Picker(FRAGMENT_SHADER, std::move(s_fun)) { }

} // namespace fio::fractals::pickers
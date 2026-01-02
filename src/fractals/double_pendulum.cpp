#include "double_pendulum.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "double_pendulum.frag"
    , 0
};

DoublePendulum::DoublePendulum(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    ChaoticFractal(FRAGMENT_SHADER, std::move(s_fun)) { }

} // namespace fio::fractals
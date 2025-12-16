#include "pendulum.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "pendulum.frag"
    , 0
};

Pendulum::Pendulum(std::function<glm::mat3x2(glm::vec2)> s_fun) :
    ComplexFractal(FRAGMENT_SHADER, std::move(s_fun)) { }

} // namespace fio::fractals
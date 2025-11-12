#include "powerbrot.hpp"

#include <memory>

#include "pickers/mandelbrot.hpp"

namespace fio::fractals {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "powerbrot.frag"
    , 0
};

Powerbrot::Powerbrot() : ComplexFractal(FRAGMENT_SHADER) {
    _picker = std::make_unique<pickers::Mandelbrot>(program());
}

} // namespace fio
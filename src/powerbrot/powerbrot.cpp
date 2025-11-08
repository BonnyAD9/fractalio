#include "powerbrot.hpp"

#include <memory>

#include "../mandel_picker/mandel_picker.hpp"

namespace fio {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "shader.frag"
    , 0
};

Powerbrot::Powerbrot() : ComplexFractal(FRAGMENT_SHADER) {
    _picker = std::make_unique<MandelPicker>(program());
}

} // namespace fio
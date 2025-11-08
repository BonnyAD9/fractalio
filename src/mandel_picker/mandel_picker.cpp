#include "mandel_picker.hpp"

namespace fio {

// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr char FRAGMENT_SHADER[]{
#embed "shader.frag"
    , 0
};

MandelPicker::MandelPicker(DFShaderProgram &program) :
    ParPicker(FRAGMENT_SHADER, program) { }

} // namespace fio
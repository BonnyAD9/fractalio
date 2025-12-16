#pragma once

#include "picker.hpp"

namespace fio::fractals::pickers {

class Grid : public Picker {
public:
    Grid(std::function<glm::mat3x2(glm::vec2)> s_fun);

    std::string describe() override { return describe_part("Grid picker"); }
};

} // namespace fio::fractals::pickers
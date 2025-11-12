#pragma once

#include <optional>

#include <glm/glm.hpp>

namespace fio::gui {

class DragListener {
public:
    virtual void drag_start(int button, int mods, glm::dvec2 pos) = 0;
    virtual void drag(glm::dvec2 pos, glm::dvec2 delta) = 0;
    virtual void drag_end(std::optional<int> button) = 0;
};

} // namespace fio
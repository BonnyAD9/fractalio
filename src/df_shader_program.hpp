#pragma once

#include <array>
#include <string_view>

#include "gl/program.hpp"

#include <glm/glm.hpp>

namespace fio {

class DFShaderProgram {
public:
    void compile(const char *vert_src, std::string_view frag_src);

    glm::ivec2 uniform_location(const char *name) const {
        return { _float.uniform_location(name),
                 _double.uniform_location(name) };
    }

    void use() {
        if (_is_double) {
            _double.use();
        } else {
            _float.use();
        }
    }

    template<typename T> void uniform(glm::ivec2 location, T value) {
        if (_is_double) {
            _float.use();
            gl::uniform(location.x, value);
            _double.use();
            gl::uniform(location.y, value);
        } else {
            _double.use();
            gl::uniform(location.y, value);
            _float.use();
            gl::uniform(location.x, value);
        }
    }

    void use_double(bool v) {
        if (v == _is_double) {
            return;
        }
        _is_double = v;
        use();
    }

    [[nodiscard]]
    bool use_double() const {
        return _is_double;
    }

private:
    gl::Program _float;
    gl::Program _double;

    bool _is_double = false;
};

} // namespace fio
#pragma once

#include <vector>
#include "iterative_fractal.hpp"
#include "../gl/program.hpp"
#include "pickers/picker_impl.hpp"
#include <glm/glm.hpp>

namespace fio::fractals {
    
class Newton : public IterativeFractal<gl::Program> {
public:
    static constexpr std::size_t DEFAULT_MAX_ROOTS = 15;

    Newton(std::function<glm::mat3x2(glm::vec2)> s_fun, std::size_t root_cnt = DEFAULT_MAX_ROOTS);
    
    std::string describe() override;
    
    USE_PICKER(_picker)

protected:
    void update_parameters(bool force) override;
    
private:
    GLint _loc_coefs;
    GLint _loc_roots;
    GLint _loc_root_cnt;
    
    pickers::PickerImpl _picker;
    
    static std::vector<glm::vec2> get_coefs(std::span<glm::vec2> roots);
};
    
}
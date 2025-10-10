#pragma once

#include <stdexcept>

#include <freetype/freetype.h>
#include <freetype/fttypes.h>

namespace fio::ft {

static inline void check_error(FT_Error err) {
    if (err != 0) {
        auto msg = FT_Error_String(err);
        throw std::runtime_error(msg);
    }
}

} // namespace fio::ft
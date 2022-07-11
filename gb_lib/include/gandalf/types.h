#ifndef __GANDALF_TYPES_H
#define __GANDALF_TYPES_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace gandalf {
    using byte = std::uint8_t;
    using signed_byte = std::int8_t;
    using word = std::uint16_t;
    using signed_word = std::int16_t;

    using ROM = std::vector<byte>;
}
#endif
#ifndef __GANDALF_TYPES_H
#define __GANDALF_TYPES_H

#include <cstdint>
#include <stdexcept>

namespace gandalf {
    using byte = std::uint8_t;
    using signed_byte = std::int8_t;
    using word = std::uint16_t;
    using signed_word = std::int16_t;

    class Exception : public std::runtime_error
    {
    public:
        Exception(std::string message) : std::runtime_error(message) {}
    };
}
#endif
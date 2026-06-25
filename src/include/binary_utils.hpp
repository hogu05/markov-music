#ifndef BINARY_UTILS_HPP
#define BINARY_UTILS_HPP

#include <cstdint>
#include <istream>
#include <ostream>

namespace binary_utils
{

inline constexpr int BITS_PER_BYTE = 8;
inline constexpr uint8_t BYTE_MASK = 0b11111111;
inline constexpr uint8_t VARLEN_CONTINUE_BIT = 0b10000000;
inline constexpr uint8_t VARLEN_DATA_MASK = 0b01111111;
inline constexpr int VARLEN_DATA_BITS = 7;
inline constexpr int VARLEN_MAX_BYTES = 4;

template <typename T> T read_big_endian(std::istream& stream)
{
    T result = 0;
    for (int i = static_cast<int>(sizeof(T)) - 1; i >= 0; --i)
    {
        result |= static_cast<T>(stream.get()) << (i * BITS_PER_BYTE);
    }
    return result;
}

uint32_t read_variable_length(std::istream& stream);

template <typename T> void write_big_endian(std::ostream& stream, T value)
{
    for (int i = static_cast<int>(sizeof(T)) - 1; i >= 0; --i)
    {
        stream.put(static_cast<char>((value >> (i * BITS_PER_BYTE)) & BYTE_MASK));
    }
}

void write_variable_length(std::ostream& stream, uint32_t value);

} // namespace binary_utils

#endif

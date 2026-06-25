#include "binary_utils.hpp"

namespace binary_utils
{

uint32_t read_variable_length(std::istream& stream)
{
    uint32_t result = 0;
    for (;;)
    {
        auto byte = static_cast<uint8_t>(stream.get());
        result = (result << VARLEN_DATA_BITS) | (byte & VARLEN_DATA_MASK);
        if ((byte & VARLEN_CONTINUE_BIT) == 0)
        {
            break;
        }
    }
    return result;
}

void write_variable_length(std::ostream& stream, uint32_t value)
{
    std::array<uint8_t, VARLEN_MAX_BYTES> bytes{};
    int count = 0;
    for (;;)
    {
        bytes.at(count++) = static_cast<uint8_t>(value & VARLEN_DATA_MASK);
        value >>= VARLEN_DATA_BITS;
        if (value == 0)
        {
            break;
        }
    }
    for (int i = count - 1; i > 0; --i)
    {
        stream.put(static_cast<char>(bytes.at(i) | VARLEN_CONTINUE_BIT));
    }
    stream.put(static_cast<char>(bytes.at(0)));
}

} // namespace binary_utils

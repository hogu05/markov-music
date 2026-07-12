#include "plain_saver.hpp"

#include <ostream>

#include "types.hpp"

void PlainSaver::save(const Notes& notes, std::ostream& stream)
{
    for (const auto& note : notes)
    {
        stream << note.pitch << " " << note.start << " " << note.duration << "\n";
    }
}

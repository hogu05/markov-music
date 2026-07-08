#include "abc_saver.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <string>

#include "types.hpp"

static constexpr int BEATS_PER_BAR = 4;
static constexpr Unit UNITS_PER_BAR = UNITS_PER_QUARTER * BEATS_PER_BAR;
static constexpr int BARS_PER_LINE = 4;
static constexpr int SEMITONES_PER_OCTAVE = 12;
static constexpr int UPPERCASE_OCTAVE = 4;
static constexpr int LOWERCASE_OCTAVE = 5;

static std::string pitch_to_abc(Pitch pitch)
{
    static constexpr std::array<const char*, SEMITONES_PER_OCTAVE> SEMITONE_NAMES = {
        "C", "^C", "D", "^D", "E", "F", "^F", "G", "^G", "A", "^A", "B"};

    int semitone = pitch % SEMITONES_PER_OCTAVE;
    int octave = (pitch / SEMITONES_PER_OCTAVE) - 1;

    std::string abc = SEMITONE_NAMES.at(semitone);

    if (octave >= LOWERCASE_OCTAVE)
    {
        for (char& c : abc)
        {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        abc += std::string(octave - LOWERCASE_OCTAVE, '\'');
    }
    else
    {
        abc += std::string(UPPERCASE_OCTAVE - octave, ',');
    }

    return abc;
}

void AbcSaver::save(const Notes& notes, std::ostream& stream)
{
    stream << "X:1" << std::endl;
    stream << "T:Generated" << std::endl;
    stream << "M:4/4" << std::endl;
    stream << "L:1/16" << std::endl;
    stream << "K:C" << std::endl;

    if (notes.empty())
    {
        return;
    }

    Notes sorted = notes;
    std::ranges::sort(sorted);

    Unit time_written = 0;
    int bars_on_line = 0;

    auto write_symbol = [&](const std::string& symbol, Unit duration, bool tie)
    {
        while (duration > 0)
        {
            Unit bar_end = ((time_written / UNITS_PER_BAR) + 1) * UNITS_PER_BAR;
            Unit chunk = std::min(duration, bar_end - time_written);

            stream << symbol;
            if (chunk != 1)
            {
                stream << chunk;
            }

            time_written += chunk;
            duration -= chunk;

            if (time_written == bar_end)
            {
                if (duration > 0 && tie)
                {
                    stream << '-';
                }
                bars_on_line++;
                if (bars_on_line % BARS_PER_LINE == 0)
                {
                    stream << "|" << std::endl;
                    bars_on_line = 0;
                }
                else
                {
                    stream << " | ";
                }
            }
        }
    };

    for (const auto& note : sorted)
    {
        if (note.start < time_written || note.duration == 0)
        {
            continue;
        }

        if (note.start > time_written)
        {
            write_symbol("z", note.start - time_written, false);
        }

        write_symbol(pitch_to_abc(note.pitch), note.duration, true);
    }

    if (time_written % UNITS_PER_BAR != 0)
    {
        stream << "|" << std::endl;
    }
}

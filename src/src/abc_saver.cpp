#include "abc_saver.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <map>
#include <ranges>
#include <string>

#include "types.hpp"

static constexpr int QUARTERS_PER_BAR = 4;
static constexpr Unit UNITS_PER_BAR = UNITS_PER_QUARTER * QUARTERS_PER_BAR;
static constexpr int BARS_PER_LINE = 4;
static constexpr int SEMITONES_PER_OCTAVE = 12;
static constexpr int UPPERCASE_OCTAVE = 4;
static constexpr int LOWERCASE_OCTAVE = 5;

static std::string pitch_to_symbol(Pitch pitch, bool tie = false)
{
    static constexpr std::array<const char*, SEMITONES_PER_OCTAVE> SEMITONE_SYMBOLS = {
        "C", "^C", "D", "^D", "E", "F", "^F", "G", "^G", "A", "^A", "B"};

    int semitone = pitch % SEMITONES_PER_OCTAVE;
    int octave = (pitch / SEMITONES_PER_OCTAVE) - 1;

    std::string symbol = SEMITONE_SYMBOLS.at(semitone);

    if (octave >= LOWERCASE_OCTAVE)
    {
        for (char& c : symbol)
        {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        symbol += std::string(octave - LOWERCASE_OCTAVE, '\'');
    }
    else
    {
        symbol += std::string(UPPERCASE_OCTAVE - octave, ',');
    }

    if (tie)
    {
        symbol += "-";
    }

    return symbol;
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

    Unit time = 0;
    int bars_on_line = 0;

    std::multimap<Unit, std::pair<Pitch, Unit>> queue;
    for (const auto& note : notes)
    {
        if (note.duration > 0)
        {
            queue.emplace(note.start, std::pair{note.pitch, note.duration});
        }
    }

    auto advance_bar = [&]()
    {
        bars_on_line++;
        if (bars_on_line == BARS_PER_LINE)
        {
            stream << " |" << std::endl;
            bars_on_line = 0;
        }
        else
        {
            stream << " | ";
        }
    };

    auto write_rest = [&](Unit duration)
    {
        while (duration > 0)
        {
            Unit bar_end = ((time / UNITS_PER_BAR) + 1) * UNITS_PER_BAR;
            Unit chunk = std::min(duration, bar_end - time);
            stream << "z" << chunk;
            time += chunk;
            duration -= chunk;
            if (time == bar_end)
            {
                advance_bar();
            }
        }
    };

    auto write_chord = [&](const std::vector<std::pair<Pitch, Unit>>& chord)
    {
        bool is_chord = chord.size() > 1;
        Unit chord_duration = std::ranges::min(chord, {}, &std::pair<Pitch, Unit>::second).second;
        Unit remaining = chord_duration;

        while (remaining > 0)
        {
            Unit bar_end = ((time / UNITS_PER_BAR) + 1) * UNITS_PER_BAR;
            Unit chunk = std::min(remaining, bar_end - time);

            std::string symbol;
            if (is_chord)
            {
                symbol = "[";
            }
            for (const auto& [pitch, duration] : chord)
            {
                bool tied = remaining > chunk || duration > chord_duration;
                symbol += pitch_to_symbol(pitch, tied);
            }
            if (is_chord)
            {
                symbol += "]";
            }

            stream << symbol << chunk;
            time += chunk;
            remaining -= chunk;

            if (time == bar_end)
            {
                advance_bar();
            }
        }

        for (const auto& [pitch, duration] : chord)
        {
            if (duration > chord_duration)
            {
                queue.emplace(time, std::pair{pitch, duration - chord_duration});
            }
        }
    };

    while (!queue.empty())
    {
        Unit start = queue.begin()->first;

        if (start > time)
        {
            write_rest(start - time);
        }

        auto [range_begin, range_end] = queue.equal_range(start);
        auto chord = std::ranges::subrange(range_begin, range_end) | std::views::values |
                     std::ranges::to<std::vector>();
        queue.erase(range_begin, range_end);

        write_chord(chord);
    }

    if (time % UNITS_PER_BAR != 0)
    {
        stream << " |" << std::endl;
    }
}

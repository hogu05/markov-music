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

static std::string pitch_to_symbol(Pitch pitch)
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

    auto write_symbol = [&](const std::string& symbol, Unit duration, bool continuation = false)
    {
        while (duration > 0)
        {
            Unit bar_end = ((time / UNITS_PER_BAR) + 1) * UNITS_PER_BAR;
            Unit chunk = std::min(duration, bar_end - time);

            stream << symbol << chunk;
            time += chunk;
            duration -= chunk;

            if (time == bar_end)
            {
                if ((duration > 0 && symbol != "z") || continuation)
                {
                    stream << '-';
                }
                bars_on_line++;
                if (bars_on_line == BARS_PER_LINE)
                {
                    stream << "|" << std::endl;
                    bars_on_line = 0;
                }
                else
                {
                    stream << " | ";
                }
            }
            else if (duration == 0 && continuation)
            {
                stream << '-';
            }
        }
    };

    std::multimap<Unit, std::pair<Pitch, Unit>> queue;
    for (const auto& note : notes)
    {
        if (note.duration > 0)
        {
            queue.emplace(note.start, std::make_pair(note.pitch, note.duration));
        }
    }

    while (!queue.empty())
    {
        Unit start = queue.begin()->first;

        if (start > time)
        {
            write_symbol("z", start - time);
        }

        auto [range_begin, range_end] = queue.equal_range(start);
        auto group = std::ranges::subrange(range_begin, range_end) | std::views::values |
                     std::ranges::to<std::vector>();
        queue.erase(range_begin, range_end);

        Unit min_duration = std::ranges::min(group, {}, &std::pair<Pitch, Unit>::second).second;

        bool has_continuation =
            std::ranges::any_of(group, [&](const auto& p) { return p.second > min_duration; });

        std::string symbol;
        if (group.size() == 1)
        {
            symbol = pitch_to_symbol(group[0].first);
        }
        else
        {
            symbol = "[";
            for (const Pitch pitch : group | std::views::keys)
            {
                symbol += pitch_to_symbol(pitch);
            }
            symbol += "]";
        }

        write_symbol(symbol, min_duration, has_continuation);

        for (const auto& [pitch, duration] : group)
        {
            if (duration > min_duration)
            {
                queue.emplace(start + min_duration, std::make_pair(pitch, duration - min_duration));
            }
        }
    }

    if (time % UNITS_PER_BAR != 0)
    {
        stream << "|" << std::endl;
    }
}

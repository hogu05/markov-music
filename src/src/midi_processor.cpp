#include "midi_processor.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "MidiFile.h"

namespace midi_processor
{
namespace
{
constexpr Unit UNITS_PER_QUARTER = 4;
constexpr int DEFAULT_TICKS_PER_QUARTER = 480;
constexpr int DEFAULT_VELOCITY = 90;

struct Note
{
    Unit start;
    Pitch pitch;
    Unit duration;
    std::strong_ordering operator<=>(const Note&) const = default;
};
} // namespace

std::vector<Token> load_tokens(const std::string& filename)
{
    std::vector<Note> notes;
    smf::MidiFile file;

    if (!file.read(filename))
    {
        std::cerr << "Error reading file: " << filename << std::endl;
        return {};
    }

    file.absoluteTicks();
    file.sortTracks();
    file.linkNotePairs();

    const double TICKS_PER_UNIT =
        file.getTicksPerQuarterNote() / static_cast<double>(UNITS_PER_QUARTER);
    auto to_unit = [&](int ticks) -> Unit
    { return static_cast<Unit>(std::round(ticks / TICKS_PER_UNIT)); };

    for (int i = 0; i < file.getTrackCount(); ++i)
    {
        for (int j = 0; j < file.getEventCount(i); ++j)
        {
            const smf::MidiEvent& event = file[i][j];

            if (event.isNoteOn() && event.isLinked() != 0)
            {
                notes.push_back({.start = to_unit(event.tick),
                                 .pitch = event.getKeyNumber(),
                                 .duration = std::max(1, to_unit(event.getTickDuration()))});
            }
        }
    }

    std::ranges::sort(notes);

    std::vector<Token> tokens;
    if (notes.empty())
    {
        return tokens;
    }

    Pitch prev_pitch = START_PITCH;

    for (size_t i = 0; i < notes.size(); ++i)
    {
        const auto& current = notes[i];

        PitchDelta delta = current.pitch - prev_pitch;

        Unit wait = END_WAIT;
        if (i + 1 < notes.size())
        {
            wait = std::max(notes[i + 1].start - current.start, 0);
        }
        tokens.push_back({.pitch_delta = delta, .duration = current.duration, .wait = wait});

        prev_pitch = current.pitch;
    }

    return tokens;
}

void save_tokens(const std::vector<Token>& tokens, const std::string& filename)
{
    smf::MidiFile file;
    file.setTicksPerQuarterNote(DEFAULT_TICKS_PER_QUARTER);

    const double TICKS_PER_UNIT =
        DEFAULT_TICKS_PER_QUARTER / static_cast<double>(UNITS_PER_QUARTER);

    Pitch current_pitch = START_PITCH;
    int current_tick_offset = 0;

    for (const auto& token : tokens)
    {
        current_pitch = std::clamp(current_pitch + token.pitch_delta, 0, MAX_PITCH);

        int start_tick = current_tick_offset;
        int end_tick = start_tick + static_cast<int>(token.duration * TICKS_PER_UNIT);

        file.addNoteOn(0, start_tick, 0, current_pitch, DEFAULT_VELOCITY);
        file.addNoteOff(0, end_tick, 0, current_pitch);

        if (token.wait != -1)
        {
            current_tick_offset += static_cast<int>(token.wait * TICKS_PER_UNIT);
        }
    }

    file.sortTracks();
    file.write(filename);
}
} // namespace midi_processor

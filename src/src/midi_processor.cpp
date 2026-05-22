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

constexpr std::array<Unit, 11> TIME_GRID = {0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

Unit snap(Unit value)
{
    return *std::ranges::min_element(TIME_GRID, [&](Unit a, Unit b)
                                     { return std::abs(a - value) < std::abs(b - value); });
}

struct Note
{
    Unit start;
    Pitch pitch;
    Unit duration;
    std::strong_ordering operator<=>(const Note&) const = default;
};
} // namespace

Track load_track(const std::string& filename)
{
    std::vector<Note> notes;
    smf::MidiFile midi_file;

    if (!midi_file.read(filename))
    {
        std::cerr << "Error reading file: " << filename << std::endl;
        return {};
    }

    midi_file.absoluteTicks();
    midi_file.sortTracks();
    midi_file.linkNotePairs();

    const double TICKS_PER_UNIT =
        midi_file.getTicksPerQuarterNote() / static_cast<double>(UNITS_PER_QUARTER);
    auto to_unit = [&](int ticks) -> Unit
    { return static_cast<Unit>(std::round(ticks / TICKS_PER_UNIT)); };

    for (int track_idx = 0; track_idx < midi_file.getTrackCount(); ++track_idx)
    {
        for (int event_idx = 0; event_idx < midi_file.getEventCount(track_idx); ++event_idx)
        {
            const smf::MidiEvent& event = midi_file[track_idx][event_idx];

            if (event.isNoteOn() && event.isLinked() != 0)
            {
                notes.push_back({.start = to_unit(event.tick),
                                 .pitch = event.getKeyNumber(),
                                 .duration = std::max(1, snap(to_unit(event.getTickDuration())))});
            }
        }
    }

    if (notes.empty())
    {
        return {};
    }

    std::ranges::sort(notes);

    Track track;

    Pitch prev_pitch = START_PITCH;

    for (size_t i = 0; i < notes.size(); ++i)
    {
        const Note& current_note = notes[i];

        PitchDelta delta = current_note.pitch - prev_pitch;

        Unit wait =
            (i + 1 < notes.size()) ? snap(notes[i + 1].start - current_note.start) : END_WAIT;
        track.push_back({.pitch_delta = delta, .duration = current_note.duration, .wait = wait});

        prev_pitch = current_note.pitch;
    }

    track.push_back(END_TOKEN);

    return track;
}

void save_track(const Track& track, const std::string& filename)
{
    smf::MidiFile midi_file;
    midi_file.setTicksPerQuarterNote(DEFAULT_TICKS_PER_QUARTER);

    const double TICKS_PER_UNIT =
        DEFAULT_TICKS_PER_QUARTER / static_cast<double>(UNITS_PER_QUARTER);

    Pitch current_pitch = START_PITCH;
    int current_tick_offset = 0;

    for (const auto& token : track)
    {
        current_pitch = std::clamp(current_pitch + token.pitch_delta, 0, MAX_PITCH);

        midi_file.addNoteOn(0, current_tick_offset, 0, current_pitch, DEFAULT_VELOCITY);
        midi_file.addNoteOff(
            0, current_tick_offset + static_cast<int>(token.duration * TICKS_PER_UNIT), 0,
            current_pitch);

        if (token.wait != END_WAIT)
        {
            current_tick_offset += static_cast<int>(token.wait * TICKS_PER_UNIT);
        }
    }

    midi_file.sortTracks();
    midi_file.write(filename);
}
} // namespace midi_processor

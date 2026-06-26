#include "types.hpp"

#include <algorithm>

constexpr std::array<Unit, 11> TIME_GRID = {0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

Unit snap(Unit value)
{
    return *std::ranges::min_element(TIME_GRID, [&](Unit a, Unit b)
                                     { return std::abs(a - value) < std::abs(b - value); });
}

Track notes_to_track(const Notes& notes)
{
    if (notes.empty())
    {
        return {};
    }

    Notes sorted = notes;
    std::ranges::sort(sorted);

    Track track;
    Pitch prev_pitch = START_PITCH;

    for (std::size_t i = 0; i < sorted.size(); ++i)
    {
        const Note& note = sorted[i];
        PitchDelta delta = note.pitch - prev_pitch;
        Unit wait = (i + 1 < sorted.size()) ? snap(sorted[i + 1].start - note.start) : 0;
        track.push_back({.pitch_delta = delta, .duration = snap(note.duration), .wait = wait});
        prev_pitch = note.pitch;
    }

    track.push_back(END_TOKEN);
    return track;
}

Notes track_to_notes(const Track& track)
{
    Notes notes;
    Pitch current_pitch = START_PITCH;
    Unit current_time = 0;

    for (const auto& token : track)
    {
        if (token == END_TOKEN)
        {
            break;
        }
        current_pitch = std::clamp(current_pitch + token.pitch_delta, 0, MAX_PITCH);
        notes.push_back(
            {.start = current_time, .pitch = current_pitch, .duration = token.duration});
        current_time += token.wait;
    }

    return notes;
}

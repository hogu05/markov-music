#include "types.hpp"

#include <algorithm>

constexpr std::array<Unit, 11> TIME_GRID = {0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32};

Unit snap(Unit value)
{
    return *std::ranges::min_element(TIME_GRID, [&](Unit a, Unit b)
                                     { return std::abs(a - value) < std::abs(b - value); });
}

Token Note::to_token(Pitch prev_pitch, Unit next_start) const
{
    return {
        .pitch_delta = pitch - prev_pitch,
        .duration = snap(duration),
        .wait = snap(next_start - start),
    };
}

Note Token::to_note(Pitch& current_pitch, Unit& current_time) const
{
    current_pitch = std::clamp(current_pitch + pitch_delta, 0, MAX_PITCH);
    Note note = {.start = current_time, .pitch = current_pitch, .duration = duration};
    current_time += wait;
    return note;
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
        Unit next_start = (i + 1 < sorted.size()) ? sorted[i + 1].start : sorted[i].start;
        track.push_back(sorted[i].to_token(prev_pitch, next_start));
        prev_pitch = sorted[i].pitch;
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
        notes.push_back(token.to_note(current_pitch, current_time));
    }

    return notes;
}

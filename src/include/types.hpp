#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>

using Unit = int;
using Pitch = int;
using PitchDelta = int;

struct Note;

struct Token
{
    PitchDelta pitch_delta;
    Unit duration;
    Unit wait;
    auto operator<=>(const Token&) const = default;
    Note to_note(Pitch& current_pitch, Unit& current_time) const;
};

using Track = std::vector<Token>;

struct Note
{
    Unit start;
    Pitch pitch;
    Unit duration;
    auto operator<=>(const Note&) const = default;
    Token to_token(Pitch prev_pitch, Unit next_start) const;
};

using Notes = std::vector<Note>;

Track notes_to_track(const Notes& notes);
Notes track_to_notes(const Track& track);

constexpr Unit UNITS_PER_QUARTER = 4;
constexpr Pitch START_PITCH = 60;
constexpr Pitch MAX_PITCH = 127;
constexpr Token EMPTY_TOKEN = Token{.pitch_delta = 0, .duration = 0, .wait = 0};
constexpr Token END_TOKEN = Token{.pitch_delta = -1, .duration = -1, .wait = -1};

#endif

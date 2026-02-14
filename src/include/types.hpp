#ifndef TYPES_HPP
#define TYPES_HPP

#include <compare>

using Unit = int;
using Pitch = int;
using PitchDelta = int;

struct Token
{
    PitchDelta pitch_delta;
    Unit duration;
    Unit wait;
    std::strong_ordering operator<=>(const Token&) const = default;
};

// TODO maybe change
constexpr Pitch START_PITCH = 60;
constexpr Pitch MAX_PITCH = 127;
constexpr Unit END_WAIT = -1;
constexpr Token START_TOKEN = Token{.pitch_delta = 0, .duration = 0, .wait = 0};
constexpr Token END_TOKEN = Token{.pitch_delta = -1, .duration = -1, .wait = -1};

#endif
